#include "instrument.h"
#include <zaber/motion/ascii/axis_settings.h>      // for AxisSettings, ascii
#include <zaber/motion/ascii/setting_constants.h>  // for ACCEL, DRIVER_CURR...
#include <unordered_map>                           // for unordered_map
#include <cstdlib>                                 // for abs


using namespace zaber::motion::ascii;


constexpr int time_per_note_ms = 1;

static std::unordered_map<char, int> notemap {
    {'A', 27450},
    {'S', 30750},
    {'D', 34500},
    {'F', 36500},
    {'H', 41000},
    {'J', 46000},
    {'K', 51800},
    {'L', 54600}
};


cInstrument::cInstrument(zaber::motion::ascii::Axis instrument): _instrument(instrument), _currentNote(0) {
    this->_instrument.getSettings().set(setting_constants::DRIVER_CURRENT_RUN, 60);
    this->_instrument.getSettings().set(setting_constants::MAXSPEED, 180000);
    this->_instrument.getSettings().set(setting_constants::ACCEL, 2000);

    this->_maxlimit = this->_instrument.getSettings().get(setting_constants::LIMIT_MAX);

    this->_instrument.home(false);
}

void cInstrument::changeNote(char newNote) {
    int accel = abs(this->convertKeytoNote(newNote) - this->_currentNote) / time_per_note_ms;
    this->_instrument.getSettings().set(setting_constants::ACCEL, accel);
    playNote(newNote);
}

void cInstrument::playNote(char note) {
    double pos = this->_instrument.getPosition();
    if (pos < this->_maxlimit * 0.1) {
        this->_direction = 1;
    } else if (pos > this->_maxlimit * 0.9) {
        this->_direction = -1;
    }

    this->_currentNote = this->convertKeytoNote(note);
    this->_instrument.moveVelocity(this->_currentNote * this->_direction);
}

void cInstrument::silence() {
    this->_instrument.stop();
    this->_currentNote = 0;
}

bool cInstrument::isPlaying() {
    return this->_currentNote != 0;
}

Axis* cInstrument::getInstrument() {
    return &this->_instrument;
}

int cInstrument::getCurrentNote() {
    return this->_currentNote;
}

int cInstrument::convertKeytoNote(char key) {
    return notemap[key];
}