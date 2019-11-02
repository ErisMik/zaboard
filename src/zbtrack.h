#include "instrument.h"
#include <vector>

enum class ZBEventType {
    START,
    NONE,
    ON,
    OFF
};

class cZBEvent {
public:
    cZBEvent(ZBEventType type, int value);
    ZBEventType type;
    int value;
};

class cZBTrack {
public:
    cZBTrack(std::vector<cInstrument>& instruments);

private:
    double secondsPerTick;
    std::vector<cInstrument> _instruments;
    std::vector<std::vector<cZBEvent>> _eventTracks;
};