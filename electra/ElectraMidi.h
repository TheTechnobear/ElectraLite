#pragma once

#include <cstdint>

// Electra sysex implmentation
// https://docs.electra.one/developers/midiimplementation.html#get-electra-info

static const uint8_t E1_Manufacturer[] = {0x00, 0x21, 0x45};

// note:
// generally 0xF0 + Manuf code + req/resp + data type + 0xF7
//
// req/resp
// upload / response (i.e. data)    : 01
// request (no data)                : 02
//
// data types
// Electra Info     : 7F
// Electra Preset   : 00
// Electra Config   : 02

// info
// response
// {
//    "versionText":"v0.9.11",
//    "versionSeq":91100,
//    "serial":"EO1-123456"
// }

enum E1_REQRES {
    E1_R_DATA   = 0x01,
    E1_R_REQ    = 0x02
};

enum E1_DATA_TYPE {
    E1_T_PRESET  = 0x01,
    E1_T_CONFIG  = 0x02,
    E1_T_INFO    = 0x7F
};


static const uint8_t E1_REQ_Info[] = { E1_R_REQ,  E1_T_INFO}; // 0xF0 0x00 0x21 0x45 0x02 0x7F 0xF7
static const uint8_t E1_RES_Info[] = { E1_R_DATA, E1_T_INFO}; // 0xF0 0x00 0x21 0x45 0x01 0x7F info-json-data 0xF7
// response
// {
//    "versionText":"v0.9.11",
//    "versionSeq":91100,
//    "serial":"EO1-123456"
// }


/// presets
// see https://docs.electra.one/developers/presetformat.html
// {
//   "version":2,
//   "name":"ADSR Test",
//   "projectId":"d8WjdwYrP3lRyyx8nEMF",
//   "pages":[
//   ],
//   "devices":[
//   ],
//   "overlays":[
//   ],
//   "groups":[
//   ],
//   "controls":[
//   ]
// }

static const uint8_t E1_UploadPreset[]       = { E1_R_DATA, E1_T_PRESET}; // 0xF0 0x00 0x21 0x45 0x01 0x00 preset-json-data 0xF7
static const uint8_t E1_REQ_DownloadPreset[] = { E1_R_REQ,  E1_T_PRESET}; // 0xF0 0x00 0x21 0x45 0x02 0x00 0xF7
static const uint8_t E1_RES_DownloadPreset[] = { E1_R_DATA, E1_T_PRESET}; // 0xF0 0x00 0x21 0x45 0x01 0x00 preset-json-data 0xF7

/// config
// see https://docs.electra.one/developers/configurationformat.html
// {
//    "version":1,
//    "router":{
//       "usbDevToMidiIo":true,
//       "usbDevToUsbHost":true,
//       "midiIoToUsbDev":true,
//       "midiIoToUsbHost":true,
//       "usbHostToMidiIo":true,
//       "usbHostToUsbDev":true,
//       "electraToMidiIo":true,
//       "electraToUsbHost":true,
//       "electraToUsbDev":true
//    },
//    "presetBanks":[
//      {
//         "id":1,
//         "name":"FX UNITS",
//         "color":"FFFFFF"
//      },
//      {
//         "id":2,
//         "name":"SAMPLERS",
//         "color":"529DEC"
//      }
//    ],
//    "usbHostAssigments":[
//      {
//         "pattern":"launchpad",
//         "port":3
//      }
//    ],
//    "midiControl":[
//      {
//         "event":"switchPreset",
//         "eventParameter":1,
//         "midiMessage":"program",
//         "parameterNumber":1
//      }
//    ]
// }

static const uint8_t E1_UploadConfig[]       = { E1_R_DATA, E1_T_CONFIG}; // 0xF0 0x00 0x21 0x45 0x01 0x02 configuration-json-data 0xF7
static const uint8_t E1_REQ_DownloadConfig[] = { E1_R_REQ,  E1_T_CONFIG}; // 0xF0 0x00 0x21 0x45 0x02 0x02 0xF7
static const uint8_t E1_RES_DownloadConfig[] = { E1_R_DATA, E1_T_CONFIG}; // 0xF0 0x00 0x21 0x45 0x01 0x02 configuration-json-data 0xF7



