#include <zaber/motion/ascii/alert_event.h>  // for ascii, motion
#include <zaber/motion/ascii/axis.h>         // for Axis
#include <zaber/motion/ascii/connection.h>   // for Connection
#include <zaber/motion/ascii/device.h>       // for Device
#include <zaber/motion/gateway/load_lib.h>   // for loadLibrary
#include <zaber/motion/library.h>            // for Library
#include <zaber/motion/log_output_mode.h>    // for LogOutputMode, LogOutput...
#include <iostream>                          // for operator<<, endl, basic_...
#include <memory>                            // for allocator_traits<>::valu...
#include <unordered_map>                     // for unordered_map
#include <vector>                            // for vector
#include "conductor.h"                       // for cConductor
#include "zkeyboard.h"                       // for CheckIsKeyDown



using namespace zaber::motion;
using namespace zaber::motion::ascii;


#if __linux__
    constexpr char LIB_LOAD_PATH [] = "/usr/local/lib/";
    constexpr char DEVICE_PORT   [] = "/dev/ttyUSB0";
#else
    constexpr char LIB_LOAD_PATH [] = "C:\\Program Files (x86)\\ZaberMotionLibrary\\lib\\";
    constexpr char DEVICE_PORT   [] = "COM4";
#endif

constexpr int ASCII_BAUD_RATE = 115200;


int main() {
    // if (loadLibrary(LIB_LOAD_PATH) != 0) {
    //     std::cout << "Lib not load " << std::endl;
    //     return 1;
    // }

    Library::setLogOutput(LogOutputMode::FILE, "zaboard.log");

    std::cout << "Starting connection to device" << std::endl;
    Connection conn = Connection::openSerialPort(DEVICE_PORT, ASCII_BAUD_RATE);

    std::cout << "Detecting devices" << std::endl;
    conn.renumberDevices();

    std::cout << "Tuning orchestra" << std::endl;
    std::vector<Device> devices = conn.detectDevices();
    cConductor conductor;

    for (int i = 0; i < (int) devices.size(); ++i) {
        for (int j = 1; j <= devices[i].getAxisCount(); ++j) {
            conductor.registerInstrumentAxis(devices[i].getAxis(j));
        }
    }
    conductor.waitForInstrumentsReady();

    const char playableKeys[] = {'Z', 'X', 'C', 'V', 'B', 'N', 'M', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K'};

    std::unordered_map<char, bool> prevKeyState;
    for (auto& key: playableKeys) {
        prevKeyState[key] = false;
    }

    while (!CheckIsKeyDown('Q')) {
        for (auto& key: playableKeys) {
            bool isKeyDown = CheckIsKeyDown(key);
            bool didPlay = false;

            /* Emulate what the keyboard events would look like */
            if (isKeyDown && prevKeyState[key]) {
                didPlay = conductor.handleKeypressPressedEvent(key);
            }
            else if (isKeyDown) {  // !prevKeyState[key]
                didPlay = conductor.handleKeypressDownEvent(key);
            }
            else if (prevKeyState[key]) {  // !isKeyDown
                conductor.handleKeypressUpEvent(key);
                didPlay = true;
            }

            if (didPlay) prevKeyState[key] = isKeyDown;
        }
    }

    std::cout << "Test Ends" << std::endl;
    conn.close();
    return 0;
}
