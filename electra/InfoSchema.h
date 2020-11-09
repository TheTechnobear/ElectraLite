//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Info data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

namespace ElectraOneInfo {
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
     * Schema for info message
     */
    struct Info {
        /**
         * serial number of unit
         */
        std::string serial;
        /**
         * numeric version number
         */
        int64_t version_seq;
        /**
         * text representation of version
         */
        std::string version_text;
    };
}

namespace nlohmann {
    void from_json(const json & j, ElectraOneInfo::Info & x);
    void to_json(json & j, const ElectraOneInfo::Info & x);

    inline void from_json(const json & j, ElectraOneInfo::Info& x) {
        x.serial = j.at("serial").get<std::string>();
        x.version_seq = j.at("versionSeq").get<int64_t>();
        x.version_text = j.at("versionText").get<std::string>();
    }

    inline void to_json(json & j, const ElectraOneInfo::Info & x) {
        j = json::object();
        j["serial"] = x.serial;
        j["versionSeq"] = x.version_seq;
        j["versionText"] = x.version_text;
    }
}
