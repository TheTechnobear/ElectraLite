#include "ElectraDevice.h"

#include <iostream>
#include <unistd.h>
#include <iomanip>

#include <sstream>

#include <readerwriterqueue.h>

#include "ElectraMidi.h"
#include "MidiDevice.h"
#include "ElectraSchema.h"


namespace ElectraLite {


static const char* E1_Midi_Device_Ctrl  = "Electra Controller Electra CTRL";
static const char* E1_Midi_Device_Port1 = "Electra Controller Electra Port 1";
static const char* E1_Midi_Device_Port2 = "Electra Controller Electra Port 2";


class ElectraMidiCallback : public MidiCallback {
public:
    ElectraMidiCallback(ElectraImpl_* parent) : parent_(parent) { ; }
    void noteOn(unsigned ch, unsigned n, unsigned v)     override;
    void noteOff(unsigned ch, unsigned n, unsigned v)    override;
    void cc(unsigned ch, unsigned cc, unsigned v)        override;
    void pitchbend(unsigned ch, int v)                   override;
    void ch_pressure(unsigned ch, unsigned v)            override;
    void process(const MidiMsg& msg) override;
    void sysex(const unsigned char* data, unsigned sz);
private:
    ElectraImpl_* parent_;
};

class ElectraImpl_ {
public:
    ElectraImpl_(void);

    ~ElectraImpl_(void) = default;
    void start(void);
    void stop(void);

    static ElectraOnePreset::Color getColour(ElectraDevice::Colour c);
    void uploadConfig(const std::string& json);
    void uploadPreset(const std::string& json);
    void requestInfo();
    void requestConfig();
    void requestPreset();

    unsigned process(void);

    void addCallback(std::shared_ptr<ElectraCallback> cb) { callbacks_.push_back(cb);}

private:
    friend class ElectraMidiCallback;

    void sendSysEx(unsigned type, const char* data, unsigned len);

    // callbacks
    void onInfo(const std::string& json);
    void onPreset(const std::string& json);
    void onConfig(const std::string& json);

    void noteOn(unsigned ch, unsigned n, unsigned v);
    void noteOff(unsigned ch, unsigned n, unsigned v);
    void cc(unsigned ch, unsigned cc, unsigned v);
    void pitchbend(unsigned ch, int v);
    void ch_pressure(unsigned ch, unsigned v);

    std::vector<std::shared_ptr<ElectraCallback>> callbacks_;

    MidiDevice device_;
    ElectraOnePreset::Preset jsonData_;
    ElectraMidiCallback midiCallback_;
};

//---------------------
ElectraImpl_::ElectraImpl_() : midiCallback_(this)
{
}

void ElectraImpl_::start() {
    device_.init(E1_Midi_Device_Ctrl, E1_Midi_Device_Ctrl);
    for (auto cb : callbacks_) {
        cb->onInit();
    }
}

ElectraOnePreset::Color  ElectraImpl_::getColour(ElectraDevice::Colour c) {
    switch (c) {
    case ElectraDevice::E_WHITE    : { return ElectraOnePreset::Color::Ffffff;}
    case ElectraDevice::E_RED      : { return ElectraOnePreset::Color::F45C51;}
    case ElectraDevice::E_ORANGE   : { return ElectraOnePreset::Color::F49500;}
    case ElectraDevice::E_BLUE     : { return ElectraOnePreset::Color::The529DEC;}
    case ElectraDevice::E_GREEN    : { return ElectraOnePreset::Color::The03A598;}
    case ElectraDevice::E_PINK     : { return ElectraOnePreset::Color::C44795;}
    default:
        ;
    }
    return ElectraOnePreset::Color::Ffffff;
}


void ElectraImpl_::stop() {
    for (auto cb : callbacks_) {
        cb->onDeinit();
    }
    device_.deinit();
}

void ElectraImpl_::sendSysEx(unsigned type, const char* data, unsigned len) {
    unsigned sz = 1 + 3 + 1 + 1 + len + 1;
    unsigned char *midi = new unsigned char[sz];
    unsigned byte = 0;
    midi[byte++] = 0xF0;

    for (auto i = 0; i < sizeof(E1_Manufacturer); i++) {
        midi[byte++] = E1_Manufacturer[i];
    }
    midi[byte++] = len > 0 ? E1_R_DATA : E1_R_REQ;
    midi[byte++] = type;

    for (auto i = 0; i < len; i++) {
        midi[byte++] = data[i];
    }
    midi[byte++] = 0xF7;

    device_.send(midi, sz);
}



void ElectraImpl_::onInfo(const std::string& json) {
    for (auto cb : callbacks_) {
        cb->onInfo(json);
    }
}

void ElectraImpl_::onPreset(const std::string& json) {
    for (auto cb : callbacks_) {
        cb->onPreset(json);
    }

}

void ElectraImpl_::onConfig(const std::string& json) {
    for (auto cb : callbacks_) {
        cb->onConfig(json);
    }
}

void ElectraImpl_::noteOn(unsigned ch, unsigned n, unsigned v) {
    for (auto cb : callbacks_) {
        cb->noteOn(ch, n, v);
    }
}

void ElectraImpl_::noteOff(unsigned ch, unsigned n, unsigned v) {
    for (auto cb : callbacks_) {
        cb->noteOff(ch, n, v);
    }
}

void ElectraImpl_::cc(unsigned ch, unsigned cc, unsigned v) {
    for (auto cb : callbacks_) {
        cb->cc(ch, cc, v);
    }
}

void ElectraImpl_::pitchbend(unsigned ch, int v) {
    for (auto cb : callbacks_) {
        cb->pitchbend(ch, v);
    }
}

void ElectraImpl_::ch_pressure(unsigned ch, unsigned v) {
    for (auto cb : callbacks_) {
        cb->ch_pressure(ch, v);
    }
}

void ElectraImpl_::uploadConfig(const std::string& json) {
    sendSysEx(E1_T_CONFIG, json.c_str(), json.length());
}

void ElectraImpl_::uploadPreset(const std::string& json) {
    sendSysEx(E1_T_PRESET_1, json.c_str(), json.length());
}

void ElectraImpl_::requestInfo() {
    sendSysEx(E1_T_INFO, nullptr, 0);
}

void ElectraImpl_::requestConfig() {
    sendSysEx(E1_T_CONFIG, nullptr, 0);
}

void ElectraImpl_::requestPreset() {
    sendSysEx(E1_T_PRESET_0, nullptr, 0);
}

unsigned ElectraImpl_::process(void) {
    unsigned count = 0;
    device_.processIn(midiCallback_);
    device_.processOut();
    return count;
}

void ElectraMidiCallback::process(const MidiMsg& msg) {
    unsigned status = msg.byte(0);
    if (status == 0xF0) {
        sysex(msg.data(), msg.size());
    } else {
        MidiCallback::process(msg);
    }
}

void ElectraMidiCallback::sysex(const unsigned char* data, unsigned sz) {
    unsigned idx = 0;
    unsigned status = data[idx++]; // FO
    unsigned man[3];
    man[0] = data[idx++];
    man[1] = data[idx++];
    man[2] = data[idx++];

    for (auto i = 0; i < 3; i++) {
        if (man[i] != E1_Manufacturer[i] ) {
            std::cerr << "sysex not from electra" << std::hex << man[0] << man[1] << man[2] << std::dec << std::endl;
            return;
        }
    }

    unsigned reqres = data[idx++];
    if (reqres != E1_R_DATA && reqres != E1_R_REQ) {
        std::cerr << "invalid msg type " << std::hex << reqres << std::dec << std::endl;
    }

    unsigned datatype = data[idx++];

    switch (reqres) {
    case E1_R_DATA : {
        unsigned jsonsz = sz - idx - 1;
        char* json = new char[jsonsz + 1];
        memcpy(json, data + idx , jsonsz);
        json[jsonsz] = 0;
        std::string jsonstr = json;
        delete [] json;

        switch (datatype) {
        case E1_T_PRESET_0 : {
            parent_->onPreset(jsonstr);
            break;
        }
        case E1_T_CONFIG : {
            parent_->onConfig(jsonstr);
            break;
        }
        case E1_T_INFO : {
            parent_->onInfo(jsonstr);
            break;
        }
        default: {
            // parent_->onError("invalid data type");
            std::cerr << "invalid data type" << std::hex << datatype << std::dec << std::endl;
        }
        }


        break;
    }
    case E1_R_REQ : {
        // not handling requests - yet
        break;
    }
    default: {
        std::cerr << "invalid msg type" << std::hex << reqres << std::dec << std::endl;
        break;
    }
    }
}


void ElectraMidiCallback::noteOn(unsigned ch, unsigned n, unsigned v) {
    parent_->noteOn(ch, n, v);
}
void ElectraMidiCallback::noteOff(unsigned ch, unsigned n, unsigned v) {
    parent_->noteOff(ch, n, v);
}

void ElectraMidiCallback::cc(unsigned ch, unsigned cc, unsigned v) {
    parent_->cc(ch, cc, v);
}

void ElectraMidiCallback::pitchbend(unsigned ch, int v) {
    parent_->pitchbend(ch, v);
}

void ElectraMidiCallback::ch_pressure(unsigned ch, unsigned v) {
    parent_->ch_pressure(ch, v);
}


//---------------------

ElectraDevice::ElectraDevice() : impl_(new ElectraImpl_()) {
}

ElectraDevice::~ElectraDevice() {
    delete impl_;
    impl_ = nullptr;
}

void ElectraDevice::start() {
    impl_->start();
}

void ElectraDevice::stop() {
    impl_->stop();
}

void ElectraDevice::uploadConfig(const std::string& json) {
    impl_->uploadConfig(json);
}

void ElectraDevice::requestInfo() {
    impl_->requestInfo();
}

void ElectraDevice::requestConfig() {
    impl_->requestConfig();
}

void ElectraDevice::requestPreset() {
    impl_->requestPreset();
}



void ElectraDevice::uploadPreset(const std::string& json) {
    impl_->uploadPreset(json);
}

void ElectraDevice::addCallback(std::shared_ptr<ElectraCallback> cb) {
    impl_->addCallback(cb);
}

ElectraOnePreset::Color ElectraDevice::getColour(ElectraDevice::Colour c) {
    return ElectraImpl_::getColour(c);
}


unsigned ElectraDevice::process() {
    return impl_->process();
}

} // namespace
