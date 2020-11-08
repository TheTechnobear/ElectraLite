#pragma once

#include <RtMidi.h>

#include <memory>
#include <vector>

#include <readerwriterqueue.h>



struct MidiMsg {
    MidiMsg() {
        data[0] = 0;
        size = 0;
    }

    MidiMsg(unsigned char status) {
        data[0] = status;
        size = 1;
    }

    MidiMsg(unsigned char status, unsigned char d1) : MidiMsg(status) {
        data[1] = d1;
        size = 2;
    }

    MidiMsg(unsigned char status, unsigned char d1, unsigned char d2) : MidiMsg(status, d1) {
        data[2] = d2;
        size = 3;
    }

    unsigned char data[3];
    unsigned size;
};


class ICallback {
public:
    ICallback () = default;
    virtual ~ICallback () = default;
    virtual void process(const MidiMsg& msg) = 0;
};


class MidiCallback : public ICallback {
public:
    virtual void noteOn(unsigned n, unsigned v)  { ; }
    virtual void noteOff(unsigned n, unsigned v) { ; }
    virtual void cc(unsigned cc, unsigned v) { ; }
    virtual void pitchbend(int v)  { ; } // +/- 8192
    virtual void ch_pressure(unsigned v) { ; }

    void process(const MidiMsg& msg) override;
};



class MidiDevice {

public:
    MidiDevice();
    virtual ~MidiDevice();
    virtual bool init(void *);
    virtual bool processIn(MidiCallback & cb);
    virtual bool processOut();
    virtual void deinit();
    virtual bool isActive();

    virtual bool midiCallback(double deltatime, std::vector<unsigned char> *message);

    bool sendCC(unsigned ch, unsigned cc, unsigned v) { return send(MidiMsg(0xB0 + ch, cc, v)); }
    bool sendNoteOn(unsigned ch, unsigned note, unsigned vel) { return send(MidiMsg(int(0x90 + ch), note, vel)); }
    bool sendNoteOff(unsigned ch, unsigned note, unsigned vel) { return send(MidiMsg(int(0x80 + ch), note, vel)); }
    bool sendRaw(char* data, unsigned sz);
protected:

    bool queueInMsg(MidiMsg &msg) { return inQueue_.try_enqueue(msg);}
    bool queueOutMsg(MidiMsg &msg) { return outQueue_.try_enqueue(msg);}

    bool nextInMsg(MidiMsg &msg) {
        return inQueue_.try_dequeue(msg);
    }

    bool nextOutMsg(MidiMsg &msg) {
        return inQueue_.try_dequeue(msg);
    }

    virtual RtMidiIn::RtMidiCallback getMidiCallback();


    bool isOutputOpen() { return (midiOutDevice_ && (virtualOpen_ || midiOutDevice_->isPortOpen())); }

    bool send(const MidiMsg &msg);

    bool active_;

    std::unique_ptr<RtMidiIn> midiInDevice_;
    std::unique_ptr<RtMidiOut> midiOutDevice_;
    bool virtualOpen_;

    moodycamel::ReaderWriterQueue<MidiMsg> outQueue_;
    moodycamel::ReaderWriterQueue<MidiMsg> inQueue_;
};
