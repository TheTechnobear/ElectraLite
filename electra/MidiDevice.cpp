#include "MidiDevice.h"

#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#define LOG_0(x) std::cerr << x << std::endl;
#define LOG_1(x) std::cerr << x << std::endl;

#ifdef __linux__
#include <alsa/asoundlib.h>
extern unsigned int portInfo(snd_seq_t *seq, snd_seq_port_info_t *pinfo, unsigned int type, int portNumber);
#endif

#ifdef __linux__
#include <alsa/asoundlib.h>

// Imported from RtMidi library.

bool findMidiPortId(unsigned &result, const std::string &portName, bool outputPort) {
    snd_seq_t *seq;
    if (snd_seq_open(&seq, "default", SND_SEQ_OPEN_INPUT, 0) < 0)
        return false;

    result = 0;
    bool success = false;
    const unsigned int type = outputPort ? SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE : SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ;

    snd_seq_addr_t addr;
    if (snd_seq_parse_address(seq, &addr, portName.c_str()) >= 0) {
        snd_seq_port_info_t *info;
        snd_seq_port_info_alloca(&info);
        unsigned count = portInfo(seq, info, type, -1);

        for (unsigned i = 0; i < count; ++i) {
            portInfo(seq, info, type, i);

            if (memcmp(&addr, snd_seq_port_info_get_addr(info), sizeof(addr)) == 0) {
                result = i;
                success = true;
                break;
            }
        }
    }

    snd_seq_close(seq);
    return success;
}

#else

bool findMidiPortId(unsigned &result, const std::string &portName, bool outputPort)
{
    RtMidiOut out;
    RtMidiIn in;
    RtMidi &rt = outputPort ? (RtMidi&)out : (RtMidi&)in;

    for (unsigned i = 0; i < rt.getPortCount(); i++) {
        if (portName.compare(rt.getPortName(i)) == 0) {
            result = i;
            return true;
        }
    }
    return false;
}

#endif // __linux__


static constexpr int MAX_QUEUE_SIZE = 128;


////////////////////////////////////////////////
void  MidiCallback::process(const MidiMsg& msg) {
    unsigned status = msg.data[0];
    unsigned data1  = msg.data[1];
    unsigned data2  = msg.data[2];

    unsigned ch = status & 0x0F;
    unsigned type = status & 0xF0;
    switch (type) {
    case 0x90: {
        if (data2 > 0) {
            noteOn(data1, data2);
        } else {
            noteOff(data1, 0);
        }
        break;
    }

    case 0x80: {
        noteOff(data1, data2);
        break;
    }
    case 0xB0: {
        cc(data1, data2);
        break;
    }
    case 0xD0: {
        ch_pressure(data1);
        break;
    }
    case 0xE0: {
        int v = (((data2 << 7) + data1)) - 8192;
        pitchbend(v);
        break;
    }
    default: {
        break;
    }
    } //switch
}



////////////////////////////////////////////////
MidiDevice::MidiDevice() :
    active_(false), inQueue_(MAX_QUEUE_SIZE),outQueue_(MAX_QUEUE_SIZE) {
}

MidiDevice::~MidiDevice() {
    deinit();
}


void MidiDeviceInCallback(double deltatime, std::vector<unsigned char> *message, void *userData) {
    MidiDevice *self = static_cast<MidiDevice *>(userData);
    self->midiCallback(deltatime, message);
}

RtMidiIn::RtMidiCallback MidiDevice::getMidiCallback() {

    return MidiDeviceInCallback;
}


bool MidiDevice::init(void *arg) {
    std::ifstream i("prefs.json");
    json prefs;
    i >> prefs; 


    if (active_) {
        deinit();
    }
    active_ = false;

    bool found = false;

    std::string input_device = prefs["input device"];
    if (!input_device.empty()) {

        try {
            midiInDevice_.reset(new RtMidiIn(RtMidi::Api::UNSPECIFIED, "MEC MIDI IN DEVICE"));
        } catch (RtMidiError &error) {
            midiInDevice_.reset();
            LOG_0("MidiDevice RtMidiIn ctor error:" << error.what());
            return false;
        }

        unsigned port;
        if (findMidiPortId(port, input_device.c_str(), false)) {
            try {
                midiInDevice_->openPort(port, "MIDI IN");
                found = true;
                LOG_1("Midi input opened : " << input_device);
            } catch (RtMidiError &error) {
                LOG_0("Midi input open error:" << error.what());
                midiInDevice_.reset();
                return false;
            }
        } else {
            LOG_0("Input device not found : [" << input_device << "]");
            LOG_0("available devices:");
            for (unsigned i = 0; i < midiInDevice_->getPortCount(); i++) {
                LOG_0("[" << midiInDevice_->getPortName(i) << "]");
            }
            midiInDevice_.reset();
            return false;
        }

        midiInDevice_->ignoreTypes(true, true, true);
        midiInDevice_->setCallback(getMidiCallback(), this);
    } //midi input

    std::string output_device = prefs["output device"];
    if (!output_device.empty()) {
        bool virt = prefs["virtual output"];
        try {
            midiOutDevice_.reset(new RtMidiOut(RtMidi::Api::UNSPECIFIED, "MEC MIDI OUT DEVICE"));
        } catch (RtMidiError &error) {
            midiOutDevice_.reset();
            LOG_0("MidiDevice RtMidiOut ctor error:" << error.what());
            return false;
        }
        if (virt) {
            try {
                midiOutDevice_->openVirtualPort("MIDI OUT");
                LOG_0("Midi virtual output created : " << output_device);
                virtualOpen_ = true;
            } catch (RtMidiError &error) {
                LOG_0("Midi virtual output create error : " << error.what());
                virtualOpen_ = false;
                midiOutDevice_.reset();
                return false;
            }
        } else {
            found = false;
            unsigned port;
            if (findMidiPortId(port, input_device.c_str(), true)) {
                try {
                    midiOutDevice_->openPort(port, "MIDI OUT");
                    LOG_0("Midi output opened  :" << output_device);
                    found = true;
                } catch (RtMidiError &error) {
                    LOG_0("Midi output create error : " << error.what());
                    midiOutDevice_.reset();
                    return false;
                }
            } else {
                LOG_0("Output device not found : [" << output_device << "]");
                LOG_0("available devices : ");
                for (unsigned i = 0; i < midiOutDevice_->getPortCount(); i++) {
                    LOG_0("[" << midiOutDevice_->getPortName(i) << "]");
                }
                midiOutDevice_.reset();
            }
        }
    } // midi output


    active_ = midiInDevice_ || midiOutDevice_;
    LOG_0("MidiDevice::init - complete, active : " <<  active_ );
    return active_;
}


bool MidiDevice::processIn(MidiCallback & cb) {
    if (!active_) return false;

    MidiMsg msg;
    while (nextInMsg(msg)) {
        cb.process(msg);
    }
    return true;
}

bool MidiDevice::processOut() {
    if (!active_) return false;

    bool lastSend = true;
    MidiMsg msg;
    while (lastSend && nextOutMsg(msg)) {
        lastSend = send(msg);
    }
    return lastSend;
}

void MidiDevice::deinit() {
    LOG_0("MidiDevice::deinit");
    if (midiInDevice_) midiInDevice_->cancelCallback();
    midiInDevice_.reset();
    active_ = false;
}

bool MidiDevice::isActive() {
    return active_;
}

bool MidiDevice::midiCallback(double, std::vector<unsigned char> *message) {
    int status = 0, data1 = 0, data2 = 0; //data3 = 0;
    unsigned int n = message->size();
    if (n > 3) LOG_0("midiCallback unexpect midi size" << n);

    status = (int) message->at(0);
    if (n > 1) data1 = (int) message->at(1);
    if (n > 2) data2 = (int) message->at(2);
//    if (n > 3) data3 = (int) message->at(3);

    MidiMsg msg(status, data1, data2);
    if(!queueInMsg(msg)) {
        LOG_0("midiCallback unable to queue msg");
    }
    return true;
}

bool MidiDevice::send(const MidiMsg &m) {
    if (midiOutDevice_ == nullptr || !isOutputOpen()) return false;
    std::vector<unsigned char> msg;

    for (int i = 0; i < m.size; i++) {
        msg.push_back(m.data[i]);
    }

    try {
        midiOutDevice_->sendMessage(&msg);
    } catch (RtMidiError &error) {
        LOG_0("MidiDevice output write error:" << error.what());
        return false;
    }
    return true;
}




