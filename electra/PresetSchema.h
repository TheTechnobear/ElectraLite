//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Preset data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann {
    template <typename T>
    struct adl_serializer<std::shared_ptr<T>> {
        static void to_json(json & j, const std::shared_ptr<T> & opt) {
            if (!opt) j = nullptr; else j = *opt;
        }

        static std::shared_ptr<T> from_json(const json & j) {
            if (j.is_null()) return std::unique_ptr<T>(); else return std::unique_ptr<T>(new T(j.get<T>()));
        }
    };
}
#endif

namespace ElectraOnePreset {
    using nlohmann::json;

    inline json get_untyped(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json & j, std::string property) {
        return get_untyped(j, property.data());
    }

    template <typename T>
    inline std::shared_ptr<T> get_optional(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<std::shared_ptr<T>>();
        }
        return std::shared_ptr<T>();
    }

    template <typename T>
    inline std::shared_ptr<T> get_optional(const json & j, std::string property) {
        return get_optional<T>(j, property.data());
    }

    /**
     * A 24-bit RGB code of the control's color. The colors are limited to six predefined colors.
     */
    enum class Color : int { C44795, F45C51, F49500, Ffffff, The03A598, The529DEC };

    /**
     * An identifier of a specific value object in the array of values.
     */
    enum class ValueId : int { Attack, Decay, L1, L2, L3, L4, R1, R2, R3, R4, Release, Sustain, Value };

    /**
     * An list of all inputs that can be assigned to a control.
     *
     * An Input provides information about an assignment of a physical control/gesture to a
     * value. An example is assigning a knob to a value of the control.
     */
    struct Input {
        /**
         * An identifier of the physical potentiometer - knob
         */
        int64_t pot_id;
        ValueId value_id;
    };

    /**
     * A mode tells adjust behaviour of the pads.
     */
    enum class PadMode : int { Momentary, Toggle };

    /**
     * A type of functional and visual representation of the control.
     */
    enum class ControlType : int { Adr, Adsr, Dx7Envelope, Fader, List, Pad, Vfader };

    /**
     * A type of the MIDI message. The type is not limited to basic MIDI messages but supports
     * their higher level implementation, such as NRPN, etc.
     */
    enum class MidiMsgType : int { Cc14, Cc7, Note, Nrpn, Program, Rpn, Start, Stop, Sysex, Tune };

    /**
     * A message object specific to the Pad control.
     *
     * A message object specific to the Fader control value.
     *
     * A message object specific to the Adsr control values.
     *
     * A message object specific to the List control value.
     */
    struct Message {
        int64_t device_id;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> off_value;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> on_value;
        std::shared_ptr<int64_t> parameter_number;
        MidiMsgType type;
        std::shared_ptr<int64_t> max;
        std::shared_ptr<int64_t> min;
    };

    /**
     * A Value object specific to the Pad control.
     *
     * A Value object specific to the Fader control.
     *
     * A Value object specific to the Adsr control.
     *
     * A Value object specific to the List control.
     */
    struct Value {
        std::shared_ptr<int64_t> default_value;
        std::shared_ptr<ValueId> id;
        /**
         * A message object specific to the Pad control.
         *
         * A message object specific to the Fader control value.
         *
         * A message object specific to the Adsr control values.
         *
         * A message object specific to the List control value.
         */
        Message message;
        std::shared_ptr<int64_t> max;
        std::shared_ptr<int64_t> min;
        std::shared_ptr<int64_t> overlay_id;
    };

    /**
     * A touch sensitive Pad that represents a switch with On and Off state.
     *
     * A Fader that can be used change a continuous value of one MIDI parameter.
     *
     * An ADSR Envelope that can be used to change continuous values of Attack, Decay, Sustain,
     * and Release MIDI parameters.
     *
     * A List that can be used to display lists of labels assigned to discrete values of a MIDI
     * parameter.
     */
    struct Control {
        std::vector<int64_t> bounds;
        std::shared_ptr<Color> color;
        int64_t control_set_id;
        int64_t id;
        std::shared_ptr<std::vector<Input>> inputs;
        std::shared_ptr<PadMode> mode;
        std::shared_ptr<std::string> name;
        int64_t page_id;
        ControlType type;
        /**
         * A list of values associated with the Pad control.
         *
         * A list of values associated with the Fader control.
         *
         * A list of values associated with the Adsr control.
         *
         * A list of values associated with the List control.
         */
        std::vector<Value> values;
    };

    /**
     * A device is a MIDI hardware or software instrument connected to one of Electra's ports.
     * It can be a hardware synth connected to a MIDI IO port, a hardware sequencer connected to
     * Electra's USB host port, or a software plugin attached to Electra's USB device ports.
     * Electra can handle up to 16 simultaneously connected devices. When working with Electra,
     * you always need to define your connected devices, you never send or receive MIDI messages
     * from port and channel directly.
     */
    struct Device {
        /**
         * A MIDI channel where the device transmits the MIDI messages.
         */
        int64_t channel;
        /**
         * A unique identifier of the device. The identifier is used in other objects to refer to a
         * particular device.
         */
        int64_t id;
        /**
         * A user-defined name of the device. The name makes it easier for users to remember and
         * identify devices.
         */
        std::string name;
        /**
         * A port number that represents the MIDI bus inside the Electra. Port 1 interconnects MIDI
         * IO port 1, USB Host port 1, USB device port 1. Port 2 interconnects MIDI IO 2, USB Host
         * port 2, and USB device port 2.
         */
        int64_t port;
    };

    /**
     * Graphical separators used to organize controls to groups by meaning. For example a group
     * 'Envelope 1' can be created for controls 'Attack', 'Decay', 'Sustain', and 'Release'.
     * Groups do not provide any other functionality than the visual grouping of controls.
     */
    struct Group {
        std::vector<int64_t> bounds;
        std::shared_ptr<Color> color;
        /**
         * A name of the group. The name is shown to the user inside the group graphics. The name is
         * trimmed to fit the size of the group.
         */
        std::string name;
        int64_t page_id;
    };

    /**
     * A text only overlay item.
     */
    struct OverlayItem {
        /**
         * A base64 encoded XBM image with 48x18 size.
         */
        std::shared_ptr<std::string> bitmap;
        /**
         * A Text label that describes the value.
         */
        std::string label;
        /**
         * A MIDI value associated with the text label.
         */
        int64_t value;
    };

    /**
     * Overlays are lists of text labels linked to particular MIDI values. Typically they are
     * assigned to selection list controls or faders. Overlays are referred to by its
     * identifier. Overlay items can be both text labels or graphical symbols represented with
     * bitmap images.
     */
    struct Overlay {
        int64_t id;
        /**
         * An list of MIDI value - text label pairs.
         */
        std::vector<OverlayItem> items;
    };

    /**
     * A page is a collection of controls and graphic objects shown on the screen at once. Each
     * page has a name to make it easier to switch between pages. The page name is shown in the
     * status bar.
     */
    struct Page {
        int64_t id;
        /**
         * A name of the page. The name makes it easier for users to get oriented in Electra presets.
         */
        std::string name;
    };

    /**
     * The Electra One preset is a definition of the preset layout and controls
     */
    struct Preset {
        /**
         * An array of controls. A control is a representation of one or more MIDI parameters or
         * messages that can be controlled by the user.
         */
        std::shared_ptr<std::vector<Control>> controls;
        /**
         * An array of devices. A device is a hardware MIDI device or software device (application,
         * VST/AU plugin) connected to the Electra One.
         */
        std::shared_ptr<std::vector<Device>> devices;
        /**
         * An array of groups. A group is a graphical separator to improve a layout of presets.
         */
        std::shared_ptr<std::vector<Group>> groups;
        /**
         * A name of the preset. The name will be shown to the user on the screen in the status bar.
         */
        std::string name;
        /**
         * An array of overlays. An overlay is a list of text labels or graphical symbols that can
         * be assigned to a control.
         */
        std::shared_ptr<std::vector<Overlay>> overlays;
        /**
         * An array of pages. A page is a collection of controls and groups displayed to the user at
         * once.
         */
        std::shared_ptr<std::vector<Page>> pages;
        /**
         * An external identifier. This id can be used to refer any external data within your
         * Electra applications. For example, the projectId is used to associate a preset within the
         * meta data, such as the preset description, author, etc., in the Electra Preset Editor
         * application.
         */
        std::shared_ptr<std::string> project_id;
        /**
         * Provides information about the version of the preset file. Electra controller uses
         * version information to distinguish between various preset file formats.
         */
        int64_t version;
    };
}

namespace nlohmann {
    void from_json(const json & j, ElectraOnePreset::Input & x);
    void to_json(json & j, const ElectraOnePreset::Input & x);

    void from_json(const json & j, ElectraOnePreset::Message & x);
    void to_json(json & j, const ElectraOnePreset::Message & x);

    void from_json(const json & j, ElectraOnePreset::Value & x);
    void to_json(json & j, const ElectraOnePreset::Value & x);

    void from_json(const json & j, ElectraOnePreset::Control & x);
    void to_json(json & j, const ElectraOnePreset::Control & x);

    void from_json(const json & j, ElectraOnePreset::Device & x);
    void to_json(json & j, const ElectraOnePreset::Device & x);

    void from_json(const json & j, ElectraOnePreset::Group & x);
    void to_json(json & j, const ElectraOnePreset::Group & x);

    void from_json(const json & j, ElectraOnePreset::OverlayItem & x);
    void to_json(json & j, const ElectraOnePreset::OverlayItem & x);

    void from_json(const json & j, ElectraOnePreset::Overlay & x);
    void to_json(json & j, const ElectraOnePreset::Overlay & x);

    void from_json(const json & j, ElectraOnePreset::Page & x);
    void to_json(json & j, const ElectraOnePreset::Page & x);

    void from_json(const json & j, ElectraOnePreset::Preset & x);
    void to_json(json & j, const ElectraOnePreset::Preset & x);

    void from_json(const json & j, ElectraOnePreset::Color & x);
    void to_json(json & j, const ElectraOnePreset::Color & x);

    void from_json(const json & j, ElectraOnePreset::ValueId & x);
    void to_json(json & j, const ElectraOnePreset::ValueId & x);

    void from_json(const json & j, ElectraOnePreset::PadMode & x);
    void to_json(json & j, const ElectraOnePreset::PadMode & x);

    void from_json(const json & j, ElectraOnePreset::ControlType & x);
    void to_json(json & j, const ElectraOnePreset::ControlType & x);

    void from_json(const json & j, ElectraOnePreset::MidiMsgType & x);
    void to_json(json & j, const ElectraOnePreset::MidiMsgType & x);

    inline void from_json(const json & j, ElectraOnePreset::Input& x) {
        x.pot_id = j.at("potId").get<int64_t>();
        x.value_id = j.at("valueId").get<ElectraOnePreset::ValueId>();
    }

    inline void to_json(json & j, const ElectraOnePreset::Input & x) {
        j = json::object();
        j["potId"] = x.pot_id;
        j["valueId"] = x.value_id;
    }

    inline void from_json(const json & j, ElectraOnePreset::Message& x) {
        x.device_id = j.at("deviceId").get<int64_t>();
        x.off_value = ElectraOnePreset::get_optional<int64_t>(j, "offValue");
        x.on_value = ElectraOnePreset::get_optional<int64_t>(j, "onValue");
        x.parameter_number = ElectraOnePreset::get_optional<int64_t>(j, "parameterNumber");
        x.type = j.at("type").get<ElectraOnePreset::MidiMsgType>();
        x.max = ElectraOnePreset::get_optional<int64_t>(j, "max");
        x.min = ElectraOnePreset::get_optional<int64_t>(j, "min");
    }

    inline void to_json(json & j, const ElectraOnePreset::Message & x) {
        j = json::object();
        j["deviceId"] = x.device_id;
        j["offValue"] = x.off_value;
        j["onValue"] = x.on_value;
        j["parameterNumber"] = x.parameter_number;
        j["type"] = x.type;
        j["max"] = x.max;
        j["min"] = x.min;
    }

    inline void from_json(const json & j, ElectraOnePreset::Value& x) {
        x.default_value = ElectraOnePreset::get_optional<int64_t>(j, "defaultValue");
        x.id = ElectraOnePreset::get_optional<ElectraOnePreset::ValueId>(j, "id");
        x.message = j.at("message").get<ElectraOnePreset::Message>();
        x.max = ElectraOnePreset::get_optional<int64_t>(j, "max");
        x.min = ElectraOnePreset::get_optional<int64_t>(j, "min");
        x.overlay_id = ElectraOnePreset::get_optional<int64_t>(j, "overlayId");
    }

    inline void to_json(json & j, const ElectraOnePreset::Value & x) {
        j = json::object();
        j["defaultValue"] = x.default_value;
        j["id"] = x.id;
        j["message"] = x.message;
        j["max"] = x.max;
        j["min"] = x.min;
        j["overlayId"] = x.overlay_id;
    }

    inline void from_json(const json & j, ElectraOnePreset::Control& x) {
        x.bounds = j.at("bounds").get<std::vector<int64_t>>();
        x.color = ElectraOnePreset::get_optional<ElectraOnePreset::Color>(j, "color");
        x.control_set_id = j.at("controlSetId").get<int64_t>();
        x.id = j.at("id").get<int64_t>();
        x.inputs = ElectraOnePreset::get_optional<std::vector<ElectraOnePreset::Input>>(j, "inputs");
        x.mode = ElectraOnePreset::get_optional<ElectraOnePreset::PadMode>(j, "mode");
        x.name = ElectraOnePreset::get_optional<std::string>(j, "name");
        x.page_id = j.at("pageId").get<int64_t>();
        x.type = j.at("type").get<ElectraOnePreset::ControlType>();
        x.values = j.at("values").get<std::vector<ElectraOnePreset::Value>>();
    }

    inline void to_json(json & j, const ElectraOnePreset::Control & x) {
        j = json::object();
        j["bounds"] = x.bounds;
        j["color"] = x.color;
        j["controlSetId"] = x.control_set_id;
        j["id"] = x.id;
        j["inputs"] = x.inputs;
        j["mode"] = x.mode;
        j["name"] = x.name;
        j["pageId"] = x.page_id;
        j["type"] = x.type;
        j["values"] = x.values;
    }

    inline void from_json(const json & j, ElectraOnePreset::Device& x) {
        x.channel = j.at("channel").get<int64_t>();
        x.id = j.at("id").get<int64_t>();
        x.name = j.at("name").get<std::string>();
        x.port = j.at("port").get<int64_t>();
    }

    inline void to_json(json & j, const ElectraOnePreset::Device & x) {
        j = json::object();
        j["channel"] = x.channel;
        j["id"] = x.id;
        j["name"] = x.name;
        j["port"] = x.port;
    }

    inline void from_json(const json & j, ElectraOnePreset::Group& x) {
        x.bounds = j.at("bounds").get<std::vector<int64_t>>();
        x.color = ElectraOnePreset::get_optional<ElectraOnePreset::Color>(j, "color");
        x.name = j.at("name").get<std::string>();
        x.page_id = j.at("pageId").get<int64_t>();
    }

    inline void to_json(json & j, const ElectraOnePreset::Group & x) {
        j = json::object();
        j["bounds"] = x.bounds;
        j["color"] = x.color;
        j["name"] = x.name;
        j["pageId"] = x.page_id;
    }

    inline void from_json(const json & j, ElectraOnePreset::OverlayItem& x) {
        x.bitmap = ElectraOnePreset::get_optional<std::string>(j, "bitmap");
        x.label = j.at("label").get<std::string>();
        x.value = j.at("value").get<int64_t>();
    }

    inline void to_json(json & j, const ElectraOnePreset::OverlayItem & x) {
        j = json::object();
        j["bitmap"] = x.bitmap;
        j["label"] = x.label;
        j["value"] = x.value;
    }

    inline void from_json(const json & j, ElectraOnePreset::Overlay& x) {
        x.id = j.at("id").get<int64_t>();
        x.items = j.at("items").get<std::vector<ElectraOnePreset::OverlayItem>>();
    }

    inline void to_json(json & j, const ElectraOnePreset::Overlay & x) {
        j = json::object();
        j["id"] = x.id;
        j["items"] = x.items;
    }

    inline void from_json(const json & j, ElectraOnePreset::Page& x) {
        x.id = j.at("id").get<int64_t>();
        x.name = j.at("name").get<std::string>();
    }

    inline void to_json(json & j, const ElectraOnePreset::Page & x) {
        j = json::object();
        j["id"] = x.id;
        j["name"] = x.name;
    }

    inline void from_json(const json & j, ElectraOnePreset::Preset& x) {
        x.controls = ElectraOnePreset::get_optional<std::vector<ElectraOnePreset::Control>>(j, "controls");
        x.devices = ElectraOnePreset::get_optional<std::vector<ElectraOnePreset::Device>>(j, "devices");
        x.groups = ElectraOnePreset::get_optional<std::vector<ElectraOnePreset::Group>>(j, "groups");
        x.name = j.at("name").get<std::string>();
        x.overlays = ElectraOnePreset::get_optional<std::vector<ElectraOnePreset::Overlay>>(j, "overlays");
        x.pages = ElectraOnePreset::get_optional<std::vector<ElectraOnePreset::Page>>(j, "pages");
        x.project_id = ElectraOnePreset::get_optional<std::string>(j, "projectId");
        x.version = j.at("version").get<int64_t>();
    }

    inline void to_json(json & j, const ElectraOnePreset::Preset & x) {
        j = json::object();
        j["controls"] = x.controls;
        j["devices"] = x.devices;
        j["groups"] = x.groups;
        j["name"] = x.name;
        j["overlays"] = x.overlays;
        j["pages"] = x.pages;
        j["projectId"] = x.project_id;
        j["version"] = x.version;
    }

    inline void from_json(const json & j, ElectraOnePreset::Color & x) {
        if (j == "C44795") x = ElectraOnePreset::Color::C44795;
        else if (j == "F45C51") x = ElectraOnePreset::Color::F45C51;
        else if (j == "F49500") x = ElectraOnePreset::Color::F49500;
        else if (j == "FFFFFF") x = ElectraOnePreset::Color::Ffffff;
        else if (j == "03A598") x = ElectraOnePreset::Color::The03A598;
        else if (j == "529DEC") x = ElectraOnePreset::Color::The529DEC;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const ElectraOnePreset::Color & x) {
        switch (x) {
            case ElectraOnePreset::Color::C44795: j = "C44795"; break;
            case ElectraOnePreset::Color::F45C51: j = "F45C51"; break;
            case ElectraOnePreset::Color::F49500: j = "F49500"; break;
            case ElectraOnePreset::Color::Ffffff: j = "FFFFFF"; break;
            case ElectraOnePreset::Color::The03A598: j = "03A598"; break;
            case ElectraOnePreset::Color::The529DEC: j = "529DEC"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, ElectraOnePreset::ValueId & x) {
        if (j == "attack") x = ElectraOnePreset::ValueId::Attack;
        else if (j == "decay") x = ElectraOnePreset::ValueId::Decay;
        else if (j == "l1") x = ElectraOnePreset::ValueId::L1;
        else if (j == "l2") x = ElectraOnePreset::ValueId::L2;
        else if (j == "l3") x = ElectraOnePreset::ValueId::L3;
        else if (j == "l4") x = ElectraOnePreset::ValueId::L4;
        else if (j == "r1") x = ElectraOnePreset::ValueId::R1;
        else if (j == "r2") x = ElectraOnePreset::ValueId::R2;
        else if (j == "r3") x = ElectraOnePreset::ValueId::R3;
        else if (j == "r4") x = ElectraOnePreset::ValueId::R4;
        else if (j == "release") x = ElectraOnePreset::ValueId::Release;
        else if (j == "sustain") x = ElectraOnePreset::ValueId::Sustain;
        else if (j == "value") x = ElectraOnePreset::ValueId::Value;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const ElectraOnePreset::ValueId & x) {
        switch (x) {
            case ElectraOnePreset::ValueId::Attack: j = "attack"; break;
            case ElectraOnePreset::ValueId::Decay: j = "decay"; break;
            case ElectraOnePreset::ValueId::L1: j = "l1"; break;
            case ElectraOnePreset::ValueId::L2: j = "l2"; break;
            case ElectraOnePreset::ValueId::L3: j = "l3"; break;
            case ElectraOnePreset::ValueId::L4: j = "l4"; break;
            case ElectraOnePreset::ValueId::R1: j = "r1"; break;
            case ElectraOnePreset::ValueId::R2: j = "r2"; break;
            case ElectraOnePreset::ValueId::R3: j = "r3"; break;
            case ElectraOnePreset::ValueId::R4: j = "r4"; break;
            case ElectraOnePreset::ValueId::Release: j = "release"; break;
            case ElectraOnePreset::ValueId::Sustain: j = "sustain"; break;
            case ElectraOnePreset::ValueId::Value: j = "value"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, ElectraOnePreset::PadMode & x) {
        if (j == "momentary") x = ElectraOnePreset::PadMode::Momentary;
        else if (j == "toggle") x = ElectraOnePreset::PadMode::Toggle;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const ElectraOnePreset::PadMode & x) {
        switch (x) {
            case ElectraOnePreset::PadMode::Momentary: j = "momentary"; break;
            case ElectraOnePreset::PadMode::Toggle: j = "toggle"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, ElectraOnePreset::ControlType & x) {
        if (j == "adr") x = ElectraOnePreset::ControlType::Adr;
        else if (j == "adsr") x = ElectraOnePreset::ControlType::Adsr;
        else if (j == "dx7envelope") x = ElectraOnePreset::ControlType::Dx7Envelope;
        else if (j == "fader") x = ElectraOnePreset::ControlType::Fader;
        else if (j == "list") x = ElectraOnePreset::ControlType::List;
        else if (j == "pad") x = ElectraOnePreset::ControlType::Pad;
        else if (j == "vfader") x = ElectraOnePreset::ControlType::Vfader;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const ElectraOnePreset::ControlType & x) {
        switch (x) {
            case ElectraOnePreset::ControlType::Adr: j = "adr"; break;
            case ElectraOnePreset::ControlType::Adsr: j = "adsr"; break;
            case ElectraOnePreset::ControlType::Dx7Envelope: j = "dx7envelope"; break;
            case ElectraOnePreset::ControlType::Fader: j = "fader"; break;
            case ElectraOnePreset::ControlType::List: j = "list"; break;
            case ElectraOnePreset::ControlType::Pad: j = "pad"; break;
            case ElectraOnePreset::ControlType::Vfader: j = "vfader"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, ElectraOnePreset::MidiMsgType & x) {
        if (j == "cc14") x = ElectraOnePreset::MidiMsgType::Cc14;
        else if (j == "cc7") x = ElectraOnePreset::MidiMsgType::Cc7;
        else if (j == "note") x = ElectraOnePreset::MidiMsgType::Note;
        else if (j == "nrpn") x = ElectraOnePreset::MidiMsgType::Nrpn;
        else if (j == "program") x = ElectraOnePreset::MidiMsgType::Program;
        else if (j == "rpn") x = ElectraOnePreset::MidiMsgType::Rpn;
        else if (j == "start") x = ElectraOnePreset::MidiMsgType::Start;
        else if (j == "stop") x = ElectraOnePreset::MidiMsgType::Stop;
        else if (j == "sysex") x = ElectraOnePreset::MidiMsgType::Sysex;
        else if (j == "tune") x = ElectraOnePreset::MidiMsgType::Tune;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const ElectraOnePreset::MidiMsgType & x) {
        switch (x) {
            case ElectraOnePreset::MidiMsgType::Cc14: j = "cc14"; break;
            case ElectraOnePreset::MidiMsgType::Cc7: j = "cc7"; break;
            case ElectraOnePreset::MidiMsgType::Note: j = "note"; break;
            case ElectraOnePreset::MidiMsgType::Nrpn: j = "nrpn"; break;
            case ElectraOnePreset::MidiMsgType::Program: j = "program"; break;
            case ElectraOnePreset::MidiMsgType::Rpn: j = "rpn"; break;
            case ElectraOnePreset::MidiMsgType::Start: j = "start"; break;
            case ElectraOnePreset::MidiMsgType::Stop: j = "stop"; break;
            case ElectraOnePreset::MidiMsgType::Sysex: j = "sysex"; break;
            case ElectraOnePreset::MidiMsgType::Tune: j = "tune"; break;
            default: throw "This should not happen";
        }
    }
}
