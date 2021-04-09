#include "ElectraDevice.h"
#include "MidiDevice.h"
#include "RtMidiDevice.h"
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <signal.h>

#include <fstream>

#include "ElectraSchema.h"


static volatile bool keepRunning = 1;


void intHandler(int dummy) {
    std::cerr << "ElectraTest intHandler called" << std::endl;
    if (!keepRunning) {
        sleep(1);
        exit(-1);
    }
    keepRunning = false;
    // device.stop();
}



ElectraLite::RtMidiDevice d;

class DumpMidiCallback : public ElectraLite::MidiCallback {
public:
    void noteOn(unsigned ch,unsigned n, unsigned v)     override    { std::cerr << "note on      : " << ch << " : " << n  << " - " << v << std::endl;}
    void noteOff(unsigned ch,unsigned n, unsigned v)    override    { std::cerr << "note off     : " << ch << " : " << n  << " - " << v << std::endl;}
    void cc(unsigned ch,unsigned cc, unsigned v)        override    { std::cerr << "cc           : " << ch << " : " << cc << " - " << v << std::endl;}
    void pitchbend(unsigned ch,int v)                   override    { std::cerr << "pitchbend    : " << ch << " : " << v  << std::endl;}
    void ch_pressure(unsigned ch,unsigned v)            override    { std::cerr << "ch_pressure  : " << ch << " : " << v  << std::endl;}
};

int main(int argc, const char * argv[]) {
    signal(SIGINT, intHandler);

    /*
    std::cerr << "testing : " << argv[1] << std::endl;
    std::ifstream i(argv[1]);

    nlohmann::json j;
    i >> j;
    ElectraOnePreset::Preset data;
    nlohmann::from_json(j, data);
    std::cerr << "project_id " << data.project_id << std::endl;
    */

    unsigned cc46=0;
    DumpMidiCallback myCallback;
    d.init(argv[1],argv[1]);
    while (keepRunning) {
        cc46=(cc46+1) %127; 
        d.sendCC(0,46,cc46);
        d.processIn(myCallback);
        d.processOut();
        sleep(1);
    }
    d.deinit();
    return 0;
}
