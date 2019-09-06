#include <zaber/motion/ascii/axis.h>  // for Axis

class cInstrument {
public:
    cInstrument(zaber::motion::ascii::Axis instrument);
    void changeNote(char newNote);
    void playNote(char note);
    void silence();
    zaber::motion::ascii::Axis* getInstrument();

private:
    zaber::motion::ascii::Axis _instrument;
    int _currentNote;
    double _maxlimit;
    int _direction = 1;
};