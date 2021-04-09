#include "ElectraDevice.h"
#include "MidiDevice.h"
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <signal.h>

#include <fstream>

// static const char* E1_Midi_Device_Ctrl  = "Electra Controller Electra CTRL";
static const char* E1_Midi_Device_Ctrl  = "mioXM HST 2";
static const char* E1_Midi_Device_Port1 = "Electra Controller Electra Port 1";
static const char* E1_Midi_Device_Port2 = "Electra Controller Electra Port 2";

static volatile bool keepRunning = 1;

class TestCallback: public ElectraLite::ElectraCallback {
public:
    virtual ~TestCallback() = default;
private:
    void onError(unsigned, const char* err) override {
        std::cerr << err << std::endl;
    }
};


void intHandler(int dummy) {
    std::cerr << "ElectraTest intHandler called" << std::endl;
    if (!keepRunning) {
        sleep(1);
        exit(-1);
    }
    keepRunning = false;
    // device.stop();
}


class DumpElectraCallback  : public ElectraLite::ElectraCallback {
public:
    virtual ~DumpElectraCallback() = default;
    void onInit() override
    { std::cerr << "DumpElectraCallback::onInit" << std::endl;}
    void onDeinit() override
    { std::cerr << "DumpElectraCallback::onDeinit" << std::endl;}
    void onError(unsigned err, const char *errStr) override
    { std::cerr << "DumpElectraCallback::onError : " << err << " - " << errStr << std::endl;}
    void onInfo(const std::string& json) override
    { std::cerr << "DumpElectraCallback::onInfo : " << json << std::endl;}
    void onPreset(const std::string& json) override
    { std::cerr << "DumpElectraCallback::onPreset : " << json << std::endl;}
    void onConfig(const std::string& json) override
    { std::cerr << "DumpElectraCallback::onConfig : " << json << std::endl;}
};





int main(int argc, const char * argv[]) {
    signal(SIGINT, intHandler);

    if(argc<2) {
        std::cerr << argv[0] << " ctrl dev"  << std::endl;
        return -1;
    }

    ElectraLite::ElectraDevice device(argv[1]);


    device.addCallback(std::make_shared<DumpElectraCallback>());
    unsigned counter = 0;
    device.start();
    device.requestInfo();
    device.requestConfig();
    device.requestPreset();
    while (keepRunning) {
        device.process();
        sleep(1);
        counter++;
    }
    device.stop();
    return 0;
}
