#include "ElectraDevice.h"
#include "MidiDevice.h"
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <signal.h>

#include <fstream>

#include "schema.h"


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



// MidiDevice d;

// class DumpMidiCallback : public MidiCallback {
// public:
//     void noteOn(unsigned n, unsigned v)     override    { std::cerr << "note on      : " << n  << " - " << v << std::endl;}
//     void noteOff(unsigned n, unsigned v)    override    { std::cerr << "note off     : " << n  << " - " << v << std::endl;}
//     void cc(unsigned cc, unsigned v)        override    { std::cerr << "cc           : " << cc << " - " << v << std::endl;  d.sendCC(0,2,v);}
//     void pitchbend(int v)                   override    { std::cerr << "pitchbend    : " << v  << std::endl;}
//     void ch_pressure(unsigned v)            override    { std::cerr << "ch_pressure  : " << v  << std::endl;}
// };


void createBounds(std::vector<int64_t>& e, unsigned x, unsigned y, unsigned h, unsigned w) {
    e.push_back(x);
    e.push_back(y);
    e.push_back(h);
    e.push_back(w);
}



void createFader(electra::TheSchema& p, unsigned x, unsigned y, unsigned h, unsigned w) {
    p.type = "fader";
    createBounds(p.bounds, x, y, h, w);
}

void createCCMessage(electra::TheMessageSchema& m, unsigned cc) {
    m.device_id = 0;
    m.type = "cc7";
    m.parameter_number = std::make_shared<int64_t>(cc);
    m.off_value = std::make_shared<int64_t>(0);
    m.on_value = std::make_shared<int64_t>(127);
    m.min = std::make_shared<int64_t>(0);
    m.max = std::make_shared<int64_t>(127);
}

void createValue(const std::string& id, electra::ValueElement& v) {
    v.id = std::make_shared<std::string>(id);
    v.min = std::make_shared<int64_t>(0);
    v.max = std::make_shared<int64_t>(100);
    // v.default_value=std::make_shared<int64_t>(50);
    // v.overlay_id=std::make_shared<int64_t>(0);
    createCCMessage(v.message, 1);
}

void createControl(electra::TheSchema& p) {
    p.id = 0;
    createFader(p, 0, 0, 100, 100);
    p.color = "red";
    p.control_set_id = std::make_shared<int64_t>(0);
    p.page_id = std::make_shared<int64_t>(0);

    // p.mode = std::make_shared<std::string >> ("momentary"); // momentary for pad
    p.name = std::make_shared<std::string>("cutoff");

    p.inputs = std::shared_ptr<std::vector<electra::InputElement>>() ;
    electra::InputElement inp;
    inp.pot_id = 0;
    inp.value_id = "vid";

    electra::ValueElement val;
    createValue("vid", val);
    p.values.push_back(val);
    p.pot_id = std::make_shared<int64_t>(0); //?
}


void buildPreset(electra::ElectraOne& p) {
    p.version = 2;
    p.name = "Orac";
    p.project_id = std::make_shared<std::string>("Orac-e1-v1");

    {
        electra::PageElement e;
        e.id = 0;
        e.name = "page1";
        p.pages = std::make_shared < std::vector<electra::PageElement>>();
        p.pages->push_back(e);
    }


    // {
    //     electra::GroupElement e;
    //     e.page_id=1;
    //     e.name="group1"
    //     create_bounds(bounds, x,y,h,w);
    //     e.color=std::make_shared<std::string>="red";
    //     p->groups=std::make_shared<std::vector<GroupElement>();
    //     p->groups.push_back(e);
    // }

    {
        electra::DeviceElement e;
        e.id = 0;
        e.name = "dev1";
        e.channel = 1;
        e.port = 1;
        p.devices = std::make_shared < std::vector<electra::DeviceElement>>();
        p.devices->push_back(e);
    }

    {
        electra::OverlayElement e;
        e.id = 0;

        electra::ItemElement items[2];
        items[0].value = 0;
        items[0].label = "OFF";
        items[1].value = 127;
        items[1].label = "ON";
        e.items.push_back(items[0]);
        e.items.push_back(items[1]);


        p.overlays = std::make_shared < std::vector<electra::OverlayElement>>();
        p.overlays->push_back(e);
    }

    {
        electra::TheSchema e;
        createControl(e);
        p.controls = std::make_shared < std::vector<electra::TheSchema>>();
        p.controls->push_back(e);
    }
}



int main(int argc, const char * argv[]) {
    signal(SIGINT, intHandler);

    // std::cerr << "testing : " << argv[1] << std::endl;
    // std::ifstream i(argv[1]);

    // nlohmann::json j;
    // i >> j;
    // electra::ElectraOne data;
    // nlohmann::from_json(j, data);
    // std::cerr << "project_id " << data.project_id << std::endl;


    electra::ElectraOne preset;
    buildPreset(preset);
    nlohmann::json j;
    nlohmann::to_json(j, preset);




    // device.addCallback(std::make_shared<TestCallback>());

    device.start();
    while (keepRunning) {
        device.process();
        sleep(1);
    }
    device.stop();
    return 0;
}
