#include "instrument.h"
#include <zaber/motion/ascii/axis_settings.h>      // for AxisSettings, ascii
#include <zaber/motion/ascii/response.h>           // for Response
#include <zaber/motion/ascii/setting_constants.h>  // for ACCEL, DRIVER_CURR...
#include <unordered_map>                           // for unordered_map


using namespace zaber::motion::ascii;

static std::unordered_map<char, int> notemap {
        {'A', 54600},
        {'S', 51800},
        {'D', 46000},
        {'F', 41000},
        {'H', 36500},
        {'J', 34500},
        {'K', 30750},
        {'L', 27450}
    };

cInstrument::cInstrument(zaber::motion::ascii::Axis instrument): _instrument(instrument) {
    this->_instrument.getSettings().set(setting_constants::DRIVER_CURRENT_RUN, 60);
    this->_instrument.getSettings().set(setting_constants::MAXSPEED, 180000);

    this->_instrument.home();

    this->_maxlimit = this->_instrument.getSettings().get(setting_constants::LIMIT_MAX);

    this->_instrument.getSettings().set(setting_constants::ACCEL, 2000);
    this->_instrument.genericCommand("move max");
}

void cInstrument::playNote(char note) {
    static int direction = 1;

    double pos = this->_instrument.getPosition();
    if ((pos < this->_maxlimit * 0.1) || (pos > this->_maxlimit * 0.9)) {
        direction = -direction;
    }

    this->_instrument.moveVelocity(notemap[note]*direction);
}

void cInstrument::silence() {
    this->_instrument.stop();
}