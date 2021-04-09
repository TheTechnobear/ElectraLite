#pragma once

#include <MidiDevice.h>

#include <libremidi/libremidi.hpp>

#include <memory>
#include <vector>

#include <readerwriterqueue.h>

#include <atomic>


namespace ElectraLite {



class LibreMidiDevice : public MidiDevice {

public:
    LibreMidiDevice();
    virtual ~LibreMidiDevice();
    bool init(const char* indevice, const char* outdevice, bool virtualOutput = false) override;
    void deinit() override;

protected:

    bool isOutputOpen() override { return (midiOutDevice_ && (virtualOpen_ || midiOutDevice_->is_port_open())); }
    bool send(const MidiMsg &msg) override;

    std::unique_ptr<libremidi::midi_in>   midiInDevice_;
    std::unique_ptr<libremidi::midi_out>  midiOutDevice_;
};


}
