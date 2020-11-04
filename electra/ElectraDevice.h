#pragma once

#include <memory>
#include <vector>

class ElectraCallback {
public:
    virtual ~ElectraCallback() = default;
    virtual void onInit()   {;}
    virtual void onFrame()  {;}
    virtual void onDeinit() {;}
    virtual void onError(unsigned err, const char *errStr) {;}
};

class ElectraImpl_;

class ElectraDevice {
public:
    ElectraDevice();
    virtual ~ElectraDevice();
    void start();
    void stop();
    unsigned process(); // call periodically
    void addCallback(std::shared_ptr<ElectraCallback>);
private:
    ElectraImpl_* impl_;
};
