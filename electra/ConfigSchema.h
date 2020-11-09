//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Config data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

namespace ElectraOneConfig {
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

    /**
     * midi control item
     */
    struct MidiControlItem {
        /**
         * Event type
         */
        std::string event;
        /**
         * Event parameter
         */
        int64_t event_parameter;
        /**
         * midi message
         */
        std::string midi_message;
        /**
         * parameterNumber
         */
        int64_t parameter_number;
    };

    /**
     * preset bank
     */
    struct PresetBank {
        /**
         * preset bank colour
         */
        std::string color;
        /**
         * preset bank id
         */
        int64_t id;
        /**
         * preset bank name
         */
        std::string name;
    };

    /**
     * Routing setup.
     */
    struct Router {
        /**
         * electra to midi din
         */
        bool electra_to_midi_io;
        /**
         * electra to midi device
         */
        bool electra_to_usb_dev;
        /**
         * electra to usb host
         */
        bool electra_to_usb_host;
        /**
         * midi din to usb device.
         */
        bool midi_io_to_usb_dev;
        /**
         * midi din to usb host
         */
        bool midi_io_to_usb_host;
        /**
         * usb device to midi din
         */
        bool usb_dev_to_midi_io;
        /**
         * usb device to usn host
         */
        bool usb_dev_to_usb_host;
        /**
         * usb host to midi din
         */
        bool usb_host_to_midi_io;
        /**
         * usb host to usb device
         */
        bool usb_host_to_usb_dev;
    };

    /**
     * An explanation about the purpose of this instance.
     */
    struct UsbHostAssigment {
        /**
         * usbHostAssigment pattern
         */
        std::string pattern;
        /**
         * usbHostAssigment midi port
         */
        int64_t port;
    };

    /**
     * Schema for config message
     */
    struct Config {
        /**
         * midi control
         */
        std::vector<MidiControlItem> midi_control;
        /**
         * preset banks
         */
        std::vector<PresetBank> preset_banks;
        /**
         * Routing setup.
         */
        Router router;
        /**
         * usbHostAssigments
         */
        std::vector<UsbHostAssigment> usb_host_assigments;
    };
}

namespace nlohmann {
    void from_json(const json & j, ElectraOneConfig::MidiControlItem & x);
    void to_json(json & j, const ElectraOneConfig::MidiControlItem & x);

    void from_json(const json & j, ElectraOneConfig::PresetBank & x);
    void to_json(json & j, const ElectraOneConfig::PresetBank & x);

    void from_json(const json & j, ElectraOneConfig::Router & x);
    void to_json(json & j, const ElectraOneConfig::Router & x);

    void from_json(const json & j, ElectraOneConfig::UsbHostAssigment & x);
    void to_json(json & j, const ElectraOneConfig::UsbHostAssigment & x);

    void from_json(const json & j, ElectraOneConfig::Config & x);
    void to_json(json & j, const ElectraOneConfig::Config & x);

    inline void from_json(const json & j, ElectraOneConfig::MidiControlItem& x) {
        x.event = j.at("event").get<std::string>();
        x.event_parameter = j.at("eventParameter").get<int64_t>();
        x.midi_message = j.at("midiMessage").get<std::string>();
        x.parameter_number = j.at("parameterNumber").get<int64_t>();
    }

    inline void to_json(json & j, const ElectraOneConfig::MidiControlItem & x) {
        j = json::object();
        j["event"] = x.event;
        j["eventParameter"] = x.event_parameter;
        j["midiMessage"] = x.midi_message;
        j["parameterNumber"] = x.parameter_number;
    }

    inline void from_json(const json & j, ElectraOneConfig::PresetBank& x) {
        x.color = j.at("color").get<std::string>();
        x.id = j.at("id").get<int64_t>();
        x.name = j.at("name").get<std::string>();
    }

    inline void to_json(json & j, const ElectraOneConfig::PresetBank & x) {
        j = json::object();
        j["color"] = x.color;
        j["id"] = x.id;
        j["name"] = x.name;
    }

    inline void from_json(const json & j, ElectraOneConfig::Router& x) {
        x.electra_to_midi_io = j.at("electraToMidiIo").get<bool>();
        x.electra_to_usb_dev = j.at("electraToUsbDev").get<bool>();
        x.electra_to_usb_host = j.at("electraToUsbHost").get<bool>();
        x.midi_io_to_usb_dev = j.at("midiIoToUsbDev").get<bool>();
        x.midi_io_to_usb_host = j.at("midiIoToUsbHost").get<bool>();
        x.usb_dev_to_midi_io = j.at("usbDevToMidiIo").get<bool>();
        x.usb_dev_to_usb_host = j.at("usbDevToUsbHost").get<bool>();
        x.usb_host_to_midi_io = j.at("usbHostToMidiIo").get<bool>();
        x.usb_host_to_usb_dev = j.at("usbHostToUsbDev").get<bool>();
    }

    inline void to_json(json & j, const ElectraOneConfig::Router & x) {
        j = json::object();
        j["electraToMidiIo"] = x.electra_to_midi_io;
        j["electraToUsbDev"] = x.electra_to_usb_dev;
        j["electraToUsbHost"] = x.electra_to_usb_host;
        j["midiIoToUsbDev"] = x.midi_io_to_usb_dev;
        j["midiIoToUsbHost"] = x.midi_io_to_usb_host;
        j["usbDevToMidiIo"] = x.usb_dev_to_midi_io;
        j["usbDevToUsbHost"] = x.usb_dev_to_usb_host;
        j["usbHostToMidiIo"] = x.usb_host_to_midi_io;
        j["usbHostToUsbDev"] = x.usb_host_to_usb_dev;
    }

    inline void from_json(const json & j, ElectraOneConfig::UsbHostAssigment& x) {
        x.pattern = j.at("pattern").get<std::string>();
        x.port = j.at("port").get<int64_t>();
    }

    inline void to_json(json & j, const ElectraOneConfig::UsbHostAssigment & x) {
        j = json::object();
        j["pattern"] = x.pattern;
        j["port"] = x.port;
    }

    inline void from_json(const json & j, ElectraOneConfig::Config& x) {
        x.midi_control = j.at("midiControl").get<std::vector<ElectraOneConfig::MidiControlItem>>();
        x.preset_banks = j.at("presetBanks").get<std::vector<ElectraOneConfig::PresetBank>>();
        x.router = j.at("router").get<ElectraOneConfig::Router>();
        x.usb_host_assigments = j.at("usbHostAssigments").get<std::vector<ElectraOneConfig::UsbHostAssigment>>();
    }

    inline void to_json(json & j, const ElectraOneConfig::Config & x) {
        j = json::object();
        j["midiControl"] = x.midi_control;
        j["presetBanks"] = x.preset_banks;
        j["router"] = x.router;
        j["usbHostAssigments"] = x.usb_host_assigments;
    }
}
