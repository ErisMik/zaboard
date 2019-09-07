#include <zaber/motion/ascii/alert_event.h>  // for ascii, motion
#include <zaber/motion/ascii/axis.h>         // for Axis
#include <zaber/motion/ascii/connection.h>   // for Connection
#include <zaber/motion/ascii/device.h>       // for Device
#include <zaber/motion/gateway/load_lib.h>   // for loadLibrary
#include <zaber/motion/library.h>            // for Library
#include <zaber/motion/log_output_mode.h>    // for LogOutputMode, LogOutput...
#include <iostream>                          // for operator<<, endl, basic_...
#include <memory>                            // for allocator_traits<>::valu...
#include <string>                            // for operator<<
#include <vector>                            // for vector
#include "instrument.h"                      // for cInstrument
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
    if (loadLibrary(LIB_LOAD_PATH) != 0) {
        std::cout << "Lib not load " << std::endl;
        return 1;
    }

    Library::setLogOutput(LogOutputMode::FILE, "zaboard.log");

    std::cout << "Starting connection to device" << std::endl;
    Connection conn = Connection::openSerialPort(DEVICE_PORT, ASCII_BAUD_RATE);

    std::cout << "Detecting devices" << std::endl;
    conn.renumberDevices();

    std::vector<Device> devices = conn.detectDevices();
    std::vector<cInstrument> instruments;

    for (int i = 0; i < (int) devices.size(); ++i) {
        for (int j = 1; j <= devices[i].getAxisCount(); ++j) {
            instruments.emplace_back(devices[i].getAxis(j));
            std::cout << "Axis: " << devices[i].getAxis(j).toString() << std::endl;
        }
    }
    for (auto& instrumentRef: instruments) {
        instrumentRef.getInstrument()->waitUntilIdle();
    }

    const char playableKeys[] = {'A', 'S', 'D', 'F', 'H', 'J', 'K', 'L'};
    std::vector<char> requests;
    requests.reserve(8);

    while (!CheckIsKeyDown('Q')) {
        requests.clear();

        for (auto& key: playableKeys) {
            if (CheckIsKeyDown(key)) {
                requests.push_back(key);
            }
        }

        for (auto& instrumentRef: instruments) {
            if (!requests.empty()) {
                instrumentRef.playNote(requests.back());
                requests.pop_back();
            } else {
                instrumentRef.silence();
            }
        }
    }

    std::cout << "Test Ends" << std::endl;
    conn.close();
    return 0;
}
