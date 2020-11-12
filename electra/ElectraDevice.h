#pragma once

#include <memory>
#include <vector>

#include "ElectraSchema.h"


namespace ElectraLite {
class ElectraCallback {
public:
    virtual ~ElectraCallback() = default;
    virtual void onInit()   {;}
    virtual void onDeinit() {;}
    virtual void onError(unsigned err, const char *errStr) {;}
    
    // device
    virtual void onInfo(const std::string& json) {;}
    virtual void onPreset(const std::string& json) {;}
    virtual void onConfig(const std::string& json) {;}

    // midi
    virtual void noteOn(unsigned ch, unsigned n, unsigned v)  { ; }
    virtual void noteOff(unsigned ch, unsigned n, unsigned v) { ; }
    virtual void cc(unsigned ch, unsigned cc, unsigned v) { ; }
    virtual void pitchbend(unsigned ch, int v)  { ; } // +/- 8192
    virtual void ch_pressure(unsigned ch, unsigned v) { ; }
};

class ElectraImpl_;

class ElectraDevice {
public:
    ElectraDevice();
    virtual ~ElectraDevice();
    void start();
    void stop();

    void uploadConfig(const std::string& json);
    void uploadPreset(const std::string& json);
    
    void requestInfo();
    void requestConfig();
    void requestPreset();

    unsigned process(); // call periodically for incoming msgs

    enum Colour {
        E_WHITE,
        E_RED,
        E_ORANGE,
        E_BLUE,
        E_GREEN,
        E_PINK,
        E_MAX_COLOR
    };
    static ElectraOnePreset::Color getColour(ElectraDevice::Colour c);


    void addCallback(std::shared_ptr<ElectraCallback>);
private:
    ElectraImpl_* impl_;
};

}
