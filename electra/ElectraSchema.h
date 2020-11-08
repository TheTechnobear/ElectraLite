//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     ElectraOne data = nlohmann::json::parse(jsonString);

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

namespace electra {
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
     * An explanation about the purpose of this instance.
     */
    struct InputElement {
        /**
         * An explanation about the purpose of this instance.
         */
        int64_t pot_id;
        /**
         * An explanation about the purpose of this instance.
         */
        std::string value_id;
    };

    /**
     * An explanation about the purpose of this instance.
     */
    struct TheMessageSchema {
        /**
         * An explanation about the purpose of this instance.
         */
        int64_t device_id;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> off_value;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> on_value;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> parameter_number;
        /**
         * An explanation about the purpose of this instance.
         */
        std::string type;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> max;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> min;
    };

    /**
     * An explanation about the purpose of this instance.
     */
    struct ValueElement {
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> default_value;
        /**
         * An explanation about the purpose of this instance.
         */
        TheMessageSchema message;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<std::string> id;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> max;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> min;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> overlay_id;
    };

    /**
     * An explanation about the purpose of this instance.
     */
    struct TheSchema {
        /**
         * An explanation about the purpose of this instance.
         */
        std::vector<int64_t> bounds;
        /**
         * An explanation about the purpose of this instance.
         */
        std::string color;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> control_set_id;
        /**
         * An explanation about the purpose of this instance.
         */
        int64_t id;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<std::vector<InputElement>> inputs;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<std::string> mode;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<std::string> name;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> page_id;
        /**
         * An explanation about the purpose of this instance.
         */
        std::string type;
        /**
         * An explanation about the purpose of this instance.
         */
        std::vector<ValueElement> values;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<int64_t> pot_id;
    };

    /**
     * An explanation about the purpose of this instance.
     */
    struct DeviceElement {
        /**
         * An explanation about the purpose of this instance.
         */
        int64_t channel;
        /**
         * An explanation about the purpose of this instance.
         */
        int64_t id;
        /**
         * An explanation about the purpose of this instance.
         */
        std::string name;
        /**
         * An explanation about the purpose of this instance.
         */
        int64_t port;
    };

    /**
     * An explanation about the purpose of this instance.
     */
    struct GroupElement {
        /**
         * An explanation about the purpose of this instance.
         */
        std::vector<int64_t> bounds;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<std::string> color;
        /**
         * An explanation about the purpose of this instance.
         */
        std::string name;
        /**
         * An explanation about the purpose of this instance.
         */
        int64_t page_id;
    };

    /**
     * An explanation about the purpose of this instance.
     */
    struct ItemElement {
        /**
         * An explanation about the purpose of this instance.
         */
        std::string label;
        /**
         * An explanation about the purpose of this instance.
         */
        int64_t value;
    };

    /**
     * An explanation about the purpose of this instance.
     */
    struct OverlayElement {
        /**
         * An explanation about the purpose of this instance.
         */
        int64_t id;
        /**
         * An explanation about the purpose of this instance.
         */
        std::vector<ItemElement> items;
    };

    /**
     * An explanation about the purpose of this instance.
     */
    struct PageElement {
        /**
         * An explanation about the purpose of this instance.
         */
        int64_t id;
        /**
         * An explanation about the purpose of this instance.
         */
        std::string name;
    };

    /**
     * The root schema comprises the entire JSON document.
     */
    struct ElectraOne {
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<std::vector<TheSchema>> controls;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<std::vector<DeviceElement>> devices;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<std::vector<GroupElement>> groups;
        /**
         * An explanation about the purpose of this instance.
         */
        std::string name;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<std::vector<OverlayElement>> overlays;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<std::vector<PageElement>> pages;
        /**
         * An explanation about the purpose of this instance.
         */
        std::shared_ptr<std::string> project_id;
        /**
         * An explanation about the purpose of this instance.
         */
        int64_t version;
    };
}

namespace nlohmann {
    void from_json(const json & j, electra::InputElement & x);
    void to_json(json & j, const electra::InputElement & x);

    void from_json(const json & j, electra::TheMessageSchema & x);
    void to_json(json & j, const electra::TheMessageSchema & x);

    void from_json(const json & j, electra::ValueElement & x);
    void to_json(json & j, const electra::ValueElement & x);

    void from_json(const json & j, electra::TheSchema & x);
    void to_json(json & j, const electra::TheSchema & x);

    void from_json(const json & j, electra::DeviceElement & x);
    void to_json(json & j, const electra::DeviceElement & x);

    void from_json(const json & j, electra::GroupElement & x);
    void to_json(json & j, const electra::GroupElement & x);

    void from_json(const json & j, electra::ItemElement & x);
    void to_json(json & j, const electra::ItemElement & x);

    void from_json(const json & j, electra::OverlayElement & x);
    void to_json(json & j, const electra::OverlayElement & x);

    void from_json(const json & j, electra::PageElement & x);
    void to_json(json & j, const electra::PageElement & x);

    void from_json(const json & j, electra::ElectraOne & x);
    void to_json(json & j, const electra::ElectraOne & x);

    inline void from_json(const json & j, electra::InputElement& x) {
        x.pot_id = j.at("potId").get<int64_t>();
        x.value_id = j.at("valueId").get<std::string>();
    }

    inline void to_json(json & j, const electra::InputElement & x) {
        j = json::object();
        j["potId"] = x.pot_id;
        j["valueId"] = x.value_id;
    }

    inline void from_json(const json & j, electra::TheMessageSchema& x) {
        x.device_id = j.at("deviceId").get<int64_t>();
        x.off_value = electra::get_optional<int64_t>(j, "offValue");
        x.on_value = electra::get_optional<int64_t>(j, "onValue");
        x.parameter_number = electra::get_optional<int64_t>(j, "parameterNumber");
        x.type = j.at("type").get<std::string>();
        x.max = electra::get_optional<int64_t>(j, "max");
        x.min = electra::get_optional<int64_t>(j, "min");
    }

    inline void to_json(json & j, const electra::TheMessageSchema & x) {
        j = json::object();
        j["deviceId"] = x.device_id;
        j["offValue"] = x.off_value;
        j["onValue"] = x.on_value;
        j["parameterNumber"] = x.parameter_number;
        j["type"] = x.type;
        j["max"] = x.max;
        j["min"] = x.min;
    }

    inline void from_json(const json & j, electra::ValueElement& x) {
        x.default_value = electra::get_optional<int64_t>(j, "defaultValue");
        x.message = j.at("message").get<electra::TheMessageSchema>();
        x.id = electra::get_optional<std::string>(j, "id");
        x.max = electra::get_optional<int64_t>(j, "max");
        x.min = electra::get_optional<int64_t>(j, "min");
        x.overlay_id = electra::get_optional<int64_t>(j, "overlayId");
    }

    inline void to_json(json & j, const electra::ValueElement & x) {
        j = json::object();
        j["defaultValue"] = x.default_value;
        j["message"] = x.message;
        j["id"] = x.id;
        j["max"] = x.max;
        j["min"] = x.min;
        j["overlayId"] = x.overlay_id;
    }

    inline void from_json(const json & j, electra::TheSchema& x) {
        x.bounds = j.at("bounds").get<std::vector<int64_t>>();
        x.color = j.at("color").get<std::string>();
        x.control_set_id = electra::get_optional<int64_t>(j, "controlSetId");
        x.id = j.at("id").get<int64_t>();
        x.inputs = electra::get_optional<std::vector<electra::InputElement>>(j, "inputs");
        x.mode = electra::get_optional<std::string>(j, "mode");
        x.name = electra::get_optional<std::string>(j, "name");
        x.page_id = electra::get_optional<int64_t>(j, "pageId");
        x.type = j.at("type").get<std::string>();
        x.values = j.at("values").get<std::vector<electra::ValueElement>>();
        x.pot_id = electra::get_optional<int64_t>(j, "potId");
    }

    inline void to_json(json & j, const electra::TheSchema & x) {
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
        j["potId"] = x.pot_id;
    }

    inline void from_json(const json & j, electra::DeviceElement& x) {
        x.channel = j.at("channel").get<int64_t>();
        x.id = j.at("id").get<int64_t>();
        x.name = j.at("name").get<std::string>();
        x.port = j.at("port").get<int64_t>();
    }

    inline void to_json(json & j, const electra::DeviceElement & x) {
        j = json::object();
        j["channel"] = x.channel;
        j["id"] = x.id;
        j["name"] = x.name;
        j["port"] = x.port;
    }

    inline void from_json(const json & j, electra::GroupElement& x) {
        x.bounds = j.at("bounds").get<std::vector<int64_t>>();
        x.color = electra::get_optional<std::string>(j, "color");
        x.name = j.at("name").get<std::string>();
        x.page_id = j.at("pageId").get<int64_t>();
    }

    inline void to_json(json & j, const electra::GroupElement & x) {
        j = json::object();
        j["bounds"] = x.bounds;
        j["color"] = x.color;
        j["name"] = x.name;
        j["pageId"] = x.page_id;
    }

    inline void from_json(const json & j, electra::ItemElement& x) {
        x.label = j.at("label").get<std::string>();
        x.value = j.at("value").get<int64_t>();
    }

    inline void to_json(json & j, const electra::ItemElement & x) {
        j = json::object();
        j["label"] = x.label;
        j["value"] = x.value;
    }

    inline void from_json(const json & j, electra::OverlayElement& x) {
        x.id = j.at("id").get<int64_t>();
        x.items = j.at("items").get<std::vector<electra::ItemElement>>();
    }

    inline void to_json(json & j, const electra::OverlayElement & x) {
        j = json::object();
        j["id"] = x.id;
        j["items"] = x.items;
    }

    inline void from_json(const json & j, electra::PageElement& x) {
        x.id = j.at("id").get<int64_t>();
        x.name = j.at("name").get<std::string>();
    }

    inline void to_json(json & j, const electra::PageElement & x) {
        j = json::object();
        j["id"] = x.id;
        j["name"] = x.name;
    }

    inline void from_json(const json & j, electra::ElectraOne& x) {
        x.controls = electra::get_optional<std::vector<electra::TheSchema>>(j, "controls");
        x.devices = electra::get_optional<std::vector<electra::DeviceElement>>(j, "devices");
        x.groups = electra::get_optional<std::vector<electra::GroupElement>>(j, "groups");
        x.name = j.at("name").get<std::string>();
        x.overlays = electra::get_optional<std::vector<electra::OverlayElement>>(j, "overlays");
        x.pages = electra::get_optional<std::vector<electra::PageElement>>(j, "pages");
        x.project_id = electra::get_optional<std::string>(j, "projectId");
        x.version = j.at("version").get<int64_t>();
    }

    inline void to_json(json & j, const electra::ElectraOne & x) {
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
}
