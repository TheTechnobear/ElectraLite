#include "ElectraDevice.h"
#include "MidiDevice.h"
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <signal.h>

#include <fstream>

#include "ElectraSchema.h"

static const char* E1_Midi_Device_Ctrl  = "Electra Controller Electra CTRL";
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

ElectraLite::ElectraDevice device(E1_Midi_Device_Ctrl);

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


void createBounds(std::vector<int64_t>& e, unsigned x, unsigned y, unsigned h, unsigned w) {
    e.push_back(x);
    e.push_back(y);
    e.push_back(h);
    e.push_back(w);
}



void createFader(ElectraOnePreset::Control& p, unsigned x, unsigned y, unsigned h, unsigned w) {
    p.type = ElectraOnePreset::ControlType::Fader;
    createBounds(p.bounds, x, y, h, w);
}

void createCCMessage(ElectraOnePreset::Message& m, unsigned cc) {
    m.device_id = 1;
    m.type = ElectraOnePreset::MidiMsgType::Cc7;
    m.parameter_number = std::make_shared<int64_t>(cc);
    m.off_value = std::make_shared<int64_t>(0);
    m.on_value = std::make_shared<int64_t>(127);
    m.min = std::make_shared<int64_t>(0);
    m.max = std::make_shared<int64_t>(127);
}

void createValue(ElectraOnePreset::Value& v) {
    v.min = std::make_shared<int64_t>(0);
    v.max = std::make_shared<int64_t>(100);
    // v.default_value = std::make_shared<int64_t>(0); //null
    // v.overlay_id = std::make_shared<int64_t>(0); // null
    createCCMessage(v.message, 1);
}


void createControl(ElectraOnePreset::Control& p) {
    p.id = 1;
    createFader(p, 0, 0, 100, 100);
    p.color = std::make_shared<ElectraOnePreset::Color>(ElectraLite::ElectraDevice::getColour(ElectraLite::ElectraDevice::E_RED));
    p.control_set_id = 1;
    p.page_id = 1;

    // p.mode = std::make_shared<ElectraOnePreset::PadMode>(ElectraOnePreset::PadMode::Momentary);
    p.name = std::make_shared<std::string>("reso");

    p.inputs = std::make_shared<std::vector<ElectraOnePreset::Input>>() ;
    ElectraOnePreset::Input inp;
    inp.pot_id = 1;
    inp.value_id = ElectraOnePreset::ValueId::Value;
    p.inputs->push_back(inp);

    ElectraOnePreset::Value val;
    val.id = std::make_shared<ElectraOnePreset::ValueId>(ElectraOnePreset::ValueId::Value);
    createValue(val);
    p.values.push_back(val);
}


void buildPreset(ElectraOnePreset::Preset& p) {
    p.version = 2;
    p.name = "Orac";
    p.project_id = std::make_shared<std::string>("Orac-e1-v1");

    {
        p.pages = std::make_shared < std::vector<ElectraOnePreset::Page>>();
        ElectraOnePreset::Page e;
        e.id = 1;
        e.name = "A1:page 1";
        p.pages->push_back(e);
    }

    {
        p.groups = std::make_shared<std::vector<ElectraOnePreset::Group>>();
        // ElectraOnePreset::Group e;
        // e.page_id = 1;
        // e.name="group1";
        // createBounds(e.bounds, 0,0,10,10);
        // p.color = std::make_shared<ElectraOnePreset::Color>(ElectraLite::ElectraDevice::getColour(ElectraLite::ElectraDevice::E_RED));
        // p.groups->push_back(e);
    }

    {
        p.devices = std::make_shared < std::vector<ElectraOnePreset::Device>>();
        ElectraOnePreset::Device e;
        e.id = 1;
        e.name = "orac";
        e.channel = 1;
        e.port = 1;
        p.devices->push_back(e);
    }

    {
        p.overlays = std::make_shared < std::vector<ElectraOnePreset::Overlay>>();

        ElectraOnePreset::Overlay e;
        e.id = 1;
        ElectraOnePreset::OverlayItem items[2];
        items[0].value = 1;
        items[0].label = "OFF";
        items[1].value = 127;
        items[1].label = "ON";
        e.items.push_back(items[0]);
        e.items.push_back(items[1]);
        p.overlays->push_back(e);
    }

    {
        p.controls = std::make_shared < std::vector<ElectraOnePreset::Control>>();
        ElectraOnePreset::Control e;
        createControl(e);
        p.controls->push_back(e);
    }
}

static void removeNullsFromJson(nlohmann::json & json) {
    if (!json.is_object() && !json.is_array())  return;
    std::vector<nlohmann::json::object_t::key_type> keys;
    for (auto &it : json.items()) {
        if (it.value().is_null())
            keys.push_back(it.key());
        else
            removeNullsFromJson(it.value());
    }
    for (auto key : keys) json.erase(key);
}

int main(int argc, const char * argv[]) {
    signal(SIGINT, intHandler);

    device.addCallback(std::make_shared<DumpElectraCallback>());
    unsigned counter = 0;
    device.start();
    device.requestInfo();
    device.requestConfig();
    device.requestPreset();
    while (keepRunning) {
        if (counter == 1) {
            ElectraOnePreset::Preset preset;
            buildPreset(preset);
            nlohmann::json j;
            nlohmann::to_json(j, preset);
            removeNullsFromJson(j);
            std::cout << j.dump(4) << std::endl;
            device.uploadPreset(j.dump());
        }
        device.process();
        sleep(1);
        counter++;
    }
    device.stop();
    return 0;
}
