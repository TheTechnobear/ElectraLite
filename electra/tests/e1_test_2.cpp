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
    m.device_id = 1;
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
    v.default_value = std::make_shared<int64_t>(0); //null
    v.overlay_id = std::make_shared<int64_t>(0); // null
    createCCMessage(v.message, 1);
}

void createControl(electra::TheSchema& p) {
    p.id = 1;
    createFader(p, 0, 0, 100, 100);
    p.color = ElectraDevice::getColour(ElectraDevice::E_RED);
    p.control_set_id = std::make_shared<int64_t>(0);
    p.page_id = std::make_shared<int64_t>(0);

    p.mode = std::make_shared<std::string >(""); // null
    // p.mode = std::make_shared<std::string >("momentary"); // momentary for pad
    p.name = std::make_shared<std::string>("reso");

    p.inputs = std::make_shared<std::vector<electra::InputElement>>() ;
    electra::InputElement inp;
    inp.pot_id = 0;
    inp.value_id = "vid";
    p.inputs->push_back(inp);

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
        p.pages = std::make_shared < std::vector<electra::PageElement>>();
        electra::PageElement e;
        e.id = 0;
        e.name = "A1:page 1";
        p.pages->push_back(e);
    }

    {
        p.groups = std::make_shared<std::vector<electra::GroupElement>>();
        // electra::GroupElement e;
        // e.page_id=1;
        // e.name="group1"
        // create_bounds(bounds, x,y,h,w);
        // e.color=std::make_shared<std::string>=ElectraDevice::getColour(ElectraDevice::E_RED);
        // p.groups.push_back(e);
    }

    {
        p.devices = std::make_shared < std::vector<electra::DeviceElement>>();
        electra::DeviceElement e;
        e.id = 1;
        e.name = "orac";
        e.channel = 1;
        e.port = 1;
        p.devices->push_back(e);
    }

    {
        p.overlays = std::make_shared < std::vector<electra::OverlayElement>>();

        electra::OverlayElement e;
        e.id = 1;
        electra::ItemElement items[2];
        items[0].value = 0;
        items[0].label = "OFF";
        items[1].value = 127;
        items[1].label = "ON";
        e.items.push_back(items[0]);
        e.items.push_back(items[1]);
        p.overlays->push_back(e);
    }

    {
        p.controls = std::make_shared < std::vector<electra::TheSchema>>();
        electra::TheSchema e;
        createControl(e);
        p.controls->push_back(e);
    }
}



int main(int argc, const char * argv[]) {
    signal(SIGINT, intHandler);



    // device.addCallback(std::make_shared<TestCallback>());
    unsigned counter = 0;
    device.start();
    while (keepRunning) {
        if (counter == 1) {
            // electra::ElectraOne preset;
            // buildPreset(preset);
            // nlohmann::json j;
            // nlohmann::to_json(j, preset);
            // std::cout << j.dump(4) << std::endl;
            // device.uploadPreset(j.dump());

            // device.requestInfo();
            device.requestConfig();
            // device.requestPreset();
        }
        device.process();
        sleep(1);
        counter++;
    }
    device.stop();
    return 0;
}
