#include "ElectraDevice.h"

#include <iostream>
#include <unistd.h>
#include <iomanip>

#include <sstream>

#include <readerwriterqueue.h>

class ElectraImpl_ {
public:
    ElectraImpl_(void);

    ~ElectraImpl_(void) = default;
    void start(void);
    void stop(void);
    unsigned process(void);

    void addCallback(std::shared_ptr<ElectraCallback> cb) {     callbacks_.push_back(cb);}

private:

    std::vector<std::shared_ptr<ElectraCallback>> callbacks_;
    // moodycamel::ReaderWriterQueue<ElectraMessage> messageQueue_;
};

//---------------------
ElectraImpl_::ElectraImpl_() 
    // :   messageQueue_(100) 
    {
}

void ElectraImpl_::start() {
}

void ElectraImpl_::stop() {
}


unsigned ElectraImpl_::process(void) {
    unsigned count =0;
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


void ElectraDevice::addCallback(std::shared_ptr<ElectraCallback> cb) {
    impl_->addCallback(cb);
}


unsigned ElectraDevice::process() {
    return impl_->process();
}
