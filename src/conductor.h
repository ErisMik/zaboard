#pragma once

#include <vector>        // for vector
#include "instrument.h"  // for cInstrument
namespace zaber { namespace motion { namespace ascii { class Axis; } } }
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

    bool handleMidiNoteOn(int trackNum, int midiNote, cNoteMap& noteMap);
    bool handleMidiNoteOff(int trackNum, int midiNote, cNoteMap& noteMap);
    bool handleMidiNoteOn(int midiNote, cNoteMap& noteMap);
    bool handleMidiNoteOff(int midiNote, cNoteMap& noteMap);

private:
    std::vector<cInstrument> _orchestra;
};