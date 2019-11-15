#pragma once

#include <zaber/motion/ascii/axis.h>

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
    int getPrevNote();

private:
    zaber::motion::ascii::Axis _instrument;
    int _currentNote;
    int _prevNote;
    double _maxlimit;
    int _direction = 1;
};
