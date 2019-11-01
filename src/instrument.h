#pragma once

#include <zaber/motion/ascii/axis.h> // for Axis

class cInstrument {
public:
    cInstrument(zaber::motion::ascii::Axis instrument);
    void changeNote(char newNote);
    void playNote(char note);
    void playMidiNote(int noteSpeed);
    void silence();
    bool isPlaying();

    zaber::motion::ascii::Axis* getInstrument();
    int convertKeytoNote(char key);
    int getCurrentNote();

private:
    zaber::motion::ascii::Axis _instrument;
    int _currentNote;
    double _maxlimit;
    int _direction = 1;
};