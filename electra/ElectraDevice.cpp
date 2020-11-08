#include "ElectraDevice.h"

#include <iostream>
#include <unistd.h>
#include <iomanip>

#include <sstream>

#include <readerwriterqueue.h>

#include "ElectraMidi.h"
#include "MidiDevice.h"
#include "schema.h"


class ElectraImpl_ {
public:
    ElectraImpl_(void);

    ~ElectraImpl_(void) = default;
    void start(void);
    void stop(void);

    void uploadConfig(const std::string& json);
    void uploadPreset(const std::string& json);
    void requestInfo();

    unsigned process(void);

    void addCallback(std::shared_ptr<ElectraCallback> cb) { callbacks_.push_back(cb);}

private:

    void sendSysEx(unsigned type, const char* data, unsigned len);

    std::vector<std::shared_ptr<ElectraCallback>> callbacks_;

    MidiDevice device_;
    electra::ElectraOne jsonData_;
    // moodycamel::ReaderWriterQueue<ElectraMessage> messageQueue_;
};

//---------------------
ElectraImpl_::ElectraImpl_()
// :   messageQueue_(100)
{
}

void ElectraImpl_::start() {
    device_.init(nullptr);
}

void ElectraImpl_::stop() {
    device_.deinit();
}

void ElectraImpl_::sendSysEx(unsigned type, const char* data, unsigned len) {
    unsigned sz = 1 + 3 + 1 + 1 + len + 1;
    char *midi = new char[sz];
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

    device_.sendRaw(midi,sz);
}



void ElectraImpl_::uploadConfig(const std::string& json) {
    sendSysEx(E1_T_CONFIG, json.c_str(), json.length());
}

void ElectraImpl_::requestInfo() {
    sendSysEx(E1_T_INFO, nullptr, 0); // request
}

void ElectraImpl_::uploadPreset(const std::string& json) {
    sendSysEx(E1_T_PRESET, json.c_str(), json.length());
}

unsigned ElectraImpl_::process(void) {
    unsigned count = 0;
    // while (messageQueue_.try_dequeue(frame)) {
    //     count++;
    //     for (auto cb : callbacks_) {
    //         // cb->onFrame();
    //     }
    // }
    return count;
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

void ElectraDevice::uploadPreset(const std::string& json) {
    impl_->uploadPreset(json);
}

void ElectraDevice::addCallback(std::shared_ptr<ElectraCallback> cb) {
    impl_->addCallback(cb);
}


unsigned ElectraDevice::process() {
    return impl_->process();
}
