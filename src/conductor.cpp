#include "conductor.h"
#include <zaber/motion/ascii/axis.h>           // for Axis
#include <zaber/motion/ascii/axis_settings.h>  // for ascii
#include <zaber/motion/ascii/device.h>         // for Device
#include <iostream>                            // for operator<<, basic_ostream
#include <string>                              // for operator<<


using namespace zaber::motion::ascii;


cConductor::cConductor(std::vector<Device> devices) {
    for (auto& deviceRef: devices) {
        for (int i = 1; i <= deviceRef.getAxisCount(); ++i) {
            this->registerInstrument(deviceRef.getAxis(i));
        }
    }
}

int cConductor::registerInstrument(cInstrument instrument) {
    std::cout << "Added: " << instrument.getInstrument()->toString() << std::endl;
    this->_orchestra.emplace_back(instrument);
    return this->_orchestra.size();
}

bool cConductor::handleKeypressDownEvent(char key) {
    for (auto& instrumentRef: this->_orchestra) {
        if (!instrumentRef.isPlaying()) {
            instrumentRef.playNote(key);
            return true;
        }
    }

    std::cout << "Unable to find free instrument for " << key << std::endl;
    return false;
}

bool cConductor::handleKeypressPressedEvent(char /*unused*/) {
    // Do nothing, for now
    return true;
}

bool cConductor::handleKeypressUpEvent(char key) {
    for (auto& instrumentRef: this->_orchestra) {
        if (instrumentRef.isPlaying()) {
            if (instrumentRef.getCurrentNote() == instrumentRef.convertKeytoNote(key)) {
                instrumentRef.silence();
                return true;
            }
        }
    }

    std::cout << "!!! No instrument playing " << key << std::endl;
    return false;
}