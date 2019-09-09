#include <vector>        // for vector
#include "instrument.h"  // for cInstrument
namespace zaber { namespace motion { namespace ascii { class Device; } } }

class cConductor {
public:
    cConductor(std::vector<zaber::motion::ascii::Device> devices);
    int registerInstrument(cInstrument instrument);

    bool handleKeypressDownEvent(char key);
    bool handleKeypressPressedEvent(char key);
    bool handleKeypressUpEvent(char key);

private:
    std::vector<cInstrument> _orchestra;
};