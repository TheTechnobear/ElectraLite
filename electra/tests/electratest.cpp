#include "ElectraDevice.h"
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <signal.h>

static volatile bool keepRunning = 1;

class TestCallback: public ElectraCallback {
public:
    virtual ~TestCallback() = default;
private:
    void onError(unsigned, const char* err) override {
        std::cerr << err << std::endl;
    }
};

ElectraDevice device;

void intHandler(int dummy) {
    std::cerr << "ElectraTest intHandler called" << std::endl;
    if(!keepRunning) {
        sleep(1);
        exit(-1);
    }
    keepRunning = false;
    device.stop();
}

int main(int argc, const char * argv[]) {
    signal(SIGINT, intHandler);

    device.addCallback(std::make_shared<TestCallback>());

    device.start();

    while(keepRunning) {
        device.process();
        sleep(1);
    }
    device.stop();
    return 0;
}
