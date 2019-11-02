#include "zbtrack.h"

cZBEvent::cZBEvent(ZBEventType type, int value) :
    type(type), value(value) {
}

cZBTrack::cZBTrack(std::vector<cInstrument>& instruments) :
    _instruments(instruments) {
    for (size_t i = 0; i < instruments.size(); ++i) {
        std::vector<cZBEvent> initEventTrack {cZBEvent(ZBEventType::START, 0)};
        this->_eventTracks.push_back(initEventTrack);
    }
}
