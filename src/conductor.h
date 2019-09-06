#include "instrument.h"
#include <vector>

class cConductor {
public:
    void handleKeypressEvent();
    int registerInstrument(cInstrument instrument);

private:
    std::vector<cInstrument> _orchestra;
};