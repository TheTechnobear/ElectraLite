#include "ElectraDevice.h"

#include <iostream>
#include <unistd.h>
#include <iomanip>

#include <sstream>

#include <readerwriterqueue.h>

#include "ElectraMidi.h"
#include "MidiDevice.h"
#include "ElectraSchema.h"


static const char* E1_Midi_Device_Ctrl  = "Electra Controller Electra CTRL";
static const char* E1_Midi_Device_Port1 = "Electra Controller Electra Port 1";
static const char* E1_Midi_Device_Port2 = "Electra Controller Electra Port 2";


class ElectraMidiCallback : public MidiCallback {
public:
    // void noteOn(unsigned n, unsigned v)     override    { std::cerr << "note on      : " << n  << " - " << v << std::endl;}
    // void noteOff(unsigned n, unsigned v)    override    { std::cerr << "note off     : " << n  << " - " << v << std::endl;}
    // void cc(unsigned cc, unsigned v)        override    { std::cerr << "cc           : " << cc << " - " << v << std::endl;}
    // void pitchbend(int v)                   override    { std::cerr << "pitchbend    : " << v  << std::endl;}
    // void ch_pressure(unsigned v)            override    { std::cerr << "ch_pressure  : " << v  << std::endl;}
    void process(const MidiMsg& msg) override;
    void sysex(const unsigned char* data, unsigned sz);
};

class ElectraImpl_ {
public:
    ElectraImpl_(void);

    ~ElectraImpl_(void) = default;
    void start(void);
    void stop(void);

    static std::string getColour(ElectraDevice::Colour c);
    void uploadConfig(const std::string& json);
    void uploadPreset(const std::string& json);
    void requestInfo();
    void requestConfig();
    void requestPreset();

    unsigned process(void);

    void addCallback(std::shared_ptr<ElectraCallback> cb) { callbacks_.push_back(cb);}

private:

    void sendSysEx(unsigned type, const char* data, unsigned len);

    std::vector<std::shared_ptr<ElectraCallback>> callbacks_;

    MidiDevice device_;
    electra::ElectraOnePreset jsonData_;
    ElectraMidiCallback midiCallback_;
    // moodycamel::ReaderWriterQueue<ElectraMessage> messageQueue_;
};

//---------------------
ElectraImpl_::ElectraImpl_()
// :   messageQueue_(100)
{
}

void ElectraImpl_::start() {
    device_.init(E1_Midi_Device_Ctrl, E1_Midi_Device_Ctrl);
}


std::string  ElectraImpl_::getColour(ElectraDevice::Colour c) {
    switch (c) {
    case ElectraDevice::E_WHITE    : { return "FFFFFF";}
    case ElectraDevice::E_RED      : { return "F45C51";}
    case ElectraDevice::E_ORANGE   : { return "F49500";}
    case ElectraDevice::E_BLUE     : { return "529DEC";}
    case ElectraDevice::E_GREEN    : { return "03A598";}
    case ElectraDevice::E_PINK     : { return "C44795";}
    default:
        ;
    }
    return "FFFFFF";

}


void ElectraImpl_::stop() {
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
    // while (messageQueue_.try_dequeue(frame)) {
    //     count++;
    //     for (auto cb : callbacks_) {
    //         // cb->onFrame();
    //     }
    // }
    return count;
}

void ElectraMidiCallback::process(const MidiMsg& msg) {
    unsigned status = msg.byte(0);
    if (status == 0xF0) {
        std::cerr << "ElectraMidiCallback::process sysex" << std::endl;
        sysex(msg.data(), msg.size());
    } else {
        std::cerr << "ElectraMidiCallback::process non-sysex " << std::hex << status << std::endl;
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

    // if(reqres==0x7f && datatype==0x7b) {
    //     // invalid from firmware 1.1.6
    //     std::cerr << "fudge info json resp" << std::endl;
    //     reqres = E1_R_DATA;
    //     datatype = E1_T_INFO;
    //     idx--;
    // }

    switch (reqres) {
    case E1_R_DATA : {
        unsigned jsonsz = sz - idx - 1;
        unsigned char* json = new unsigned char[jsonsz + 1];
        memcpy(json, data + idx , jsonsz);
        json[jsonsz] = 0;

        switch (datatype) {
        case E1_T_PRESET_0 : {
            std::cerr << "preset json: " << json << std::endl;
            break;
        }
        case E1_T_CONFIG : {
            std::cerr << "config json: " << json << std::endl;
            break;
        }
        case E1_T_INFO : {
            std::cerr << "info json: " << json << std::endl;
            break;
        }
        default: {
            std::cerr << "invalid data type" << std::hex << datatype << std::dec << std::endl;
        }
        }


        delete [] json;
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

std::string  ElectraDevice::getColour(ElectraDevice::Colour c) {
    return ElectraImpl_::getColour(c);
}


unsigned ElectraDevice::process() {
    return impl_->process();
}
