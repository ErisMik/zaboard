#include "../conductor.h"
#include "../notemap.h"
#include "../zbtrack.h"
#include <cmath>
#include <midifile/MidiFile.h>

constexpr double DEFAULT_SEC_PER_TICK = 93e-3;
constexpr double MS_PER_SEC = 1000.0;
constexpr int TRACK = 0;

constexpr double CHANGE_NOTE_CONST = 1.0;
constexpr double NOTE_INSTRUMENT_CONST = 1.0;

double costForNoteOnInstrument(int newNote, cInstrument& instrument) {
    return NOTE_INSTRUMENT_CONST * (0 /* TODO: Eric Mikulin, figure out the math to go here */);
}

double costToChangeNote(int newNote, int prevNote) {
    return CHANGE_NOTE_CONST * (newNote - prevNote);
}

double costToAssignNote(int note, cInstrument& instrument) {
    double totalCost = costToChangeNote(note, /*FIXME*/ note) + costForNoteOnInstrument(note, instrument);
    return totalCost;
}

void assignNoteToDevice(int note, std::vector<cInstrument>& instruments) {
    std::vector<double> costs;
    costs.reserve(instruments.size());
    for (auto& instrument : instruments) {
        costs.push_back(costToAssignNote(note, instrument));
    }

    int smallestIdx = std::min_element(costs.begin(), costs.end()) - costs.begin();

    // TODO: Eric Mikulin
}

inline int intNote(smf::MidiEvent* mev) {
    constexpr size_t noteIdx = 1;
    return (int)(*mev)[noteIdx];
}

void preprocessMIDIFile(std::string filename, std::vector<cInstrument>& instruments) {
    smf::MidiFile midifile;
    midifile.read(filename);
    midifile.joinTracks();

    const int tpq = midifile.getTicksPerQuarterNote();
    int deltaTick;
    double secondsPerTick = DEFAULT_SEC_PER_TICK;

    cZBTrack zbTrack(instruments);

    smf::MidiEvent* mev;
    for (int event = 0; event < midifile[TRACK].size(); ++event) {
        mev = &midifile[TRACK][event];

        if (event == 0) {
            deltaTick = mev->tick;
        } else {
            deltaTick = mev->tick - midifile[TRACK][event - 1].tick;
        }

        if (mev->isTempo())
            secondsPerTick = mev->getTempoSPT(tpq);
        else if (mev->isNoteOn()) {
            // conductor.handleMidiNoteOn((int)(*mev)[1], midiNoteMap);
        } else if (mev->isNoteOff()) {
            // conductor.handleMidiNoteOff((int)(*mev)[1], midiNoteMap);
        }

        int msSleepTime = MS_PER_SEC * secondsPerTick * deltaTick;
    }
}
