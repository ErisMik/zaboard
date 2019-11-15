#include "conductor.h"
#include "notemap.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <limits>
#include <zaber/motion/ascii/axis.h>
#include <zaber/motion/ascii/axis_settings.h>

using namespace zaber::motion::ascii;

double costToAssignNote(int newNote, cInstrument& instrument) {
    double totalCost = 1.0 * abs(newNote - instrument.getPrevNote());

    if (instrument.getPrevNote() == 0) {
        totalCost = 0;
    }

    if (instrument.isPlaying()) {
        totalCost = -1;
    }

    return totalCost;
}

size_t cConductor::pickBestInstrument(int newNote) {
    double lowestCost = std::numeric_limits<double>::max();
    size_t bestInstrument = 0;

    size_t idx = 0;
    for (auto& instrumentRef : this->_orchestra) {
        double cost = costToAssignNote(newNote, instrumentRef);
        std::cout << cost << " ";
        if (cost < lowestCost && cost >= 0) {
            lowestCost = cost;
            bestInstrument = idx;
        }
        idx++;
    }
    std::cout << "{" << bestInstrument << "}" << std::endl;

    return bestInstrument;
}

/* For some odd reason this constructor results in the interface ID become funky AF.
    No doubt this is something to do with copying the Connection class around. Will investigate. */
// cConductor::cConductor(std::vector<Device> devices) {
//     for (auto& deviceRef: devices) {
//         for (int i = 1; i <= deviceRef.getAxisCount(); ++i) {
//             this->registerInstrumentAxis(deviceRef.getAxis(i));
//         }
//     }

//     for (auto& instrumentRef: this->_orchestra) {
//         instrumentRef.getInstrument()->waitUntilIdle();
//     }
// }

int cConductor::registerInstrument(cInstrument instrument) {
    std::cout << "Added: " << instrument.getInstrument()->toString() << std::endl;
    this->_orchestra.push_back(instrument);
    return this->_orchestra.size();
}

int cConductor::registerInstrumentAxis(Axis axis) {
    std::cout << "Added: " << axis.toString() << std::endl;
    this->_orchestra.emplace_back(axis);
    return this->_orchestra.size();
}

void cConductor::waitForInstrumentsReady() {
    for (auto& instrumentRef : this->_orchestra) {
        instrumentRef.getInstrument()->waitUntilIdle();
    }
}

bool cConductor::handleKeypressDownEvent(char key) {
    size_t bestInstrumentIdx = pickBestInstrument(this->_orchestra[0].convertKeytoNote(key));
    cInstrument& bestInstrument = this->_orchestra[bestInstrumentIdx];
    if (!bestInstrument.isPlaying()) {
        bestInstrument.playNote(key);
        return true;
    }

    std::cout << "Unable to find free instrument for " << key << std::endl;
    return false;
}

bool cConductor::handleKeypressPressedEvent(char /*unused*/) {
    // Do nothing, for now
    return true;
}

bool cConductor::handleKeypressUpEvent(char key) {
    for (auto& instrumentRef : this->_orchestra) {
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

bool cConductor::handleMidiNoteOn(int midiNote, cNoteMap& noteMap) {
    size_t bestInstrumentIdx = pickBestInstrument(midiNote);
    cInstrument& bestInstrument = this->_orchestra[bestInstrumentIdx];
    if (!bestInstrument.isPlaying()) {
        int speed = noteMap.getSpeed(midiNote, "");

        bestInstrument.playMidiNote(speed);
        return true;
    }

    std::cout << "No instrumentfree for " << midiNote << std::endl;
    return false;
}

bool cConductor::handleMidiNoteOff(int midiNote, cNoteMap& noteMap) {
    for (auto& instrumentRef : this->_orchestra) {
        if (instrumentRef.isPlaying()) {
            int speed = noteMap.getSpeed(midiNote, "");

            if (instrumentRef.getCurrentNote() == speed) {
                instrumentRef.silence();
                return true;
            }
        }
    }

    std::cout << "!!! No instrument playing " << midiNote << std::endl;
    return false;
}