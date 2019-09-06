#include "zkeyboard.h"
#include <zaber/motion/ascii.h>
#include <zaber/motion/ascii/setting_constants.h>
#include <chrono>
#include <iostream>
#include <thread>


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
constexpr int BINARY_BAUD_RATE = 9600;
constexpr double TEST_VEL = 30.0;
constexpr int MOVE_TIME = 100;


int SPEEDS[] = { 54600, 51800, 46000, 41000, 36500, 34500, 30750, 27450 };
int VOL[5] = { 20, 30, 40, 50, 60 };


int main() {
    if (loadLibrary(LIB_LOAD_PATH) != 0) {
        std::cout << "Lib not load " << std::endl;
        return 1;
    }

    std::cout << "Starting connection to device" << std::endl;
    Connection conn = Connection::openSerialPort(DEVICE_PORT, ASCII_BAUD_RATE);

    std::cout << "Detecting devices" << std::endl;
    conn.renumberDevices();
    std::vector<Device> devices = conn.detectDevices();
    std::vector<Axis> axes;
    std::vector<double> maxlimits;
    for (int i = 0; i < (int) devices.size(); ++i) {
        for (int j = 1; j <= devices[i].getAxisCount(); ++j) {
            axes.push_back(devices[i].getAxis(j));
            std::cout << "Axis " << i << ": " << devices[i].getAxis(j).toString() << std::endl;
        }
    }

    std::cout << "Setting up devices" << std::endl;
    for (int i = 0; i < (int) axes.size(); ++i) {
        axes[i].getSettings().set(setting_constants::DRIVER_CURRENT_RUN, 60);
        axes[i].getSettings().set(setting_constants::MAXSPEED, 180000);
        axes[i].home(false);

        maxlimits.push_back(axes[i].getSettings().get(setting_constants::LIMIT_MAX));
        axes[i].getSettings().set(setting_constants::ACCEL, 2000);
        //axes[i].moveAbsolute(maxlimits[i] / 2);
        axes[i].genericCommand("move max");
    }
    for (int i = 0; i < (int) axes.size(); ++i) {
        axes[i].waitUntilIdle();
    }

    int direction[] = { 1 };
    // int speed_size = sizeof(VOL) / sizeof(VOL[0]);

    while (!CheckIsKeyDown('Q')) {
        int note = 0;
        int requests[8] = { 0 };

        if (CheckIsKeyDown('A')) {
            requests[note] = (SPEEDS[7]);
            note++;
        }
        if (CheckIsKeyDown('S')) {
            requests[note] = (SPEEDS[6]);
            note++;
        }
        if (CheckIsKeyDown('D')) {
            requests[note] = (SPEEDS[5]);
            note++;
        }
        if (CheckIsKeyDown('F')) {
            requests[note] = (SPEEDS[4]);
            note++;
        }
        if (CheckIsKeyDown('H')) {
            requests[note] = (SPEEDS[3]);
            note++;
        }
        if (CheckIsKeyDown('J')) {
            requests[note] = (SPEEDS[2]);
            note++;
        }
        if (CheckIsKeyDown('K')) {
            requests[note] = (SPEEDS[1]);
            note++;
        }
        if (CheckIsKeyDown('L')) {
            requests[note] = (SPEEDS[0]);
            note++;
        }
        for (int i = 0; i < (int) axes.size(); ++i) {
            double pos = axes[i].getPosition();
            if (pos < maxlimits[i] * 0.1) {
                direction[i] = 1;
            }
            else if (pos > maxlimits[i] * 0.9) {
                direction[i] = -1;
            }
            if (requests[i] != 0) {
                axes[i].moveVelocity(requests[i] * direction[i]);
            }
            else {
                axes[i].stop();
            }
        }
    }

    std::cout << "Test Ends" << std::endl;
    conn.close();
    return 0;
}
