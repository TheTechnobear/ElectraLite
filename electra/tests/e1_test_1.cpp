#include "ElectraDevice.h"
#include "MidiDevice.h"
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <signal.h>

#include <fstream>

#include "ElectraSchema.h"


static volatile bool keepRunning = 1;

class TestCallback: public ElectraCallback {
public:
    virtual ~TestCallback() = default;
private:
    void onError(unsigned, const char* err) override {
        std::cerr << err << std::endl;
    }
};

ElectraDevice device;

void intHandler(int dummy) {
    std::cerr << "ElectraTest intHandler called" << std::endl;
    if (!keepRunning) {
        sleep(1);
        exit(-1);
    }
    keepRunning = false;
    // device.stop();
}



MidiDevice d;

class DumpMidiCallback : public MidiCallback {
public:
    void noteOn(unsigned n, unsigned v)     override    { std::cerr << "note on      : " << n  << " - " << v << std::endl;}
    void noteOff(unsigned n, unsigned v)    override    { std::cerr << "note off     : " << n  << " - " << v << std::endl;}
    void cc(unsigned cc, unsigned v)        override    { std::cerr << "cc           : " << cc << " - " << v << std::endl;}
    void pitchbend(int v)                   override    { std::cerr << "pitchbend    : " << v  << std::endl;}
    void ch_pressure(unsigned v)            override    { std::cerr << "ch_pressure  : " << v  << std::endl;}
};

static const char* E1_Midi_Device_Ctrl  = "Electra Controller Electra CTRL";
static const char* E1_Midi_Device_Port1 = "Electra Controller Electra Port 1";
static const char* E1_Midi_Device_Port2 = "Electra Controller Electra Port 2";

int main(int argc, const char * argv[]) {
    signal(SIGINT, intHandler);

    std::cerr << "testing : " << argv[1] << std::endl;
    std::ifstream i(argv[1]);

    nlohmann::json j;
    i >> j;
    electra::ElectraOne data;
    nlohmann::from_json(j, data);
    std::cerr << "project_id " << data.project_id << std::endl;

    DumpMidiCallback myCallback;
    d.init(E1_Midi_Device_Port1,E1_Midi_Device_Port1);
    while (keepRunning) {
        d.processIn(myCallback);
        d.processOut();
        sleep(1);
    }
    d.deinit();
    return 0;
}