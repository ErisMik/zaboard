#pragma once

#include "instrument.h" // for cInstrument
#include <vector> // for vector

namespace zaber {
namespace motion {
    namespace ascii {
        class Axis;
    }
}
}
class cNoteMap;

class cConductor {
public:
    // cConductor(std::vector<zaber::motion::ascii::Device> devices);  // See conductor.cpp
    int registerInstrument(cInstrument instrument);
    int registerInstrumentAxis(zaber::motion::ascii::Axis axis);

    void waitForInstrumentsReady();

    bool handleKeypressDownEvent(char key);
    bool handleKeypressPressedEvent(char key);
    bool handleKeypressUpEvent(char key);

    bool handleMidiNoteOn(int midiNote, cNoteMap& noteMap);
    bool handleMidiNoteOff(int midiNote, cNoteMap& noteMap);

private:
    std::vector<cInstrument> _orchestra;
};
