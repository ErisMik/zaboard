#include "conductor.h"
#include "notemap.h"
#include "zkeyboard.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <midifile/MidiEvent.h>
#include <midifile/MidiEventList.h>
#include <midifile/MidiFile.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <zaber/motion/ascii/axis.h>
#include <zaber/motion/ascii/axis_settings.h>
#include <zaber/motion/ascii/connection.h>
#include <zaber/motion/ascii/device.h>
#include <zaber/motion/device_db_source_type.h>
#include <zaber/motion/library.h>
#include <zaber/motion/units.h>

using namespace zaber::motion;
using namespace zaber::motion::ascii;

#if __linux__
constexpr char DEVICE_PORT[] = "/dev/ttyUSB1";
#else
constexpr char DEVICE_PORT[] = "COM4";
#endif

constexpr int ASCII_BAUD_RATE = 115200;
constexpr double DEFAULT_SEC_PER_TICK = 93e-3;
constexpr double MS_PER_SEC = 1000.0;
constexpr int TRACK = 0;

int playMidiFile(std::string filename) {
    std::cout << "Starting connection to device" << std::endl;
    Connection conn = Connection::openSerialPort(DEVICE_PORT, ASCII_BAUD_RATE);

    std::cout << "Detecting devices" << std::endl;
    conn.renumberDevices();

    std::cout << "Tuning orchestra" << std::endl;
    std::vector<Device> devices = conn.detectDevices();
    cConductor conductor;

    for (int i = 0; i < (int)devices.size(); ++i) {
        for (int j = 1; j <= devices[i].getAxisCount(); ++j) {
            conductor.registerInstrumentAxis(devices[i].getAxis(j));
        }
    }
    conductor.waitForInstrumentsReady();

    cNoteMap midiNoteMap = cNoteMap::mapFromCSV();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << "Now Playing: " << filename << std::endl;
    smf::MidiFile midifile;
    midifile.read(filename);
    midifile.joinTracks();
    const int tpq = midifile.getTicksPerQuarterNote();

    int deltaTick;
    double sec_per_tick = DEFAULT_SEC_PER_TICK;

    smf::MidiEvent* mev;
    for (int event = 0; event < midifile[TRACK].size(); ++event) {
        mev = &midifile[TRACK][event];

        if (event == 0) {
            deltaTick = mev->tick;
        } else {
            deltaTick = mev->tick - midifile[TRACK][event - 1].tick;
        }

        if (mev->isTempo())
            sec_per_tick = mev->getTempoSPT(tpq);
        else if (mev->isNoteOn()) {
            conductor.handleMidiNoteOn((int)(*mev)[1], midiNoteMap);
        } else if (mev->isNoteOff()) {
            conductor.handleMidiNoteOff((int)(*mev)[1], midiNoteMap);
        }

        int ms_sleep_time = MS_PER_SEC * sec_per_tick * deltaTick;
        if (deltaTick) std::this_thread::sleep_for(std::chrono::milliseconds(ms_sleep_time));
    }

    return 0;
}

int playLiveMidiBoard() {
    std::cout << "Feature not implemented yet" << std::endl;
    return 1;
}

int playLiveKeybaord() {
    std::cout << "Starting connection to device" << std::endl;
    Connection conn = Connection::openSerialPort(DEVICE_PORT, ASCII_BAUD_RATE);

    std::cout << "Detecting devices" << std::endl;
    conn.renumberDevices();

    std::cout << "Tuning orchestra" << std::endl;
    std::vector<Device> devices = conn.detectDevices();
    cConductor conductor;

    for (int i = 0; i < (int)devices.size(); ++i) {
        for (int j = 1; j <= devices[i].getAxisCount(); ++j) {
            conductor.registerInstrumentAxis(devices[i].getAxis(j));
        }
    }
    conductor.waitForInstrumentsReady();

    const char playableKeys[] = {'Z', 'X', 'C', 'V', 'B', 'N', 'M', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K'};

    std::unordered_map<char, bool> prevKeyState;
    for (auto& key : playableKeys) {
        prevKeyState[key] = false;
    }

    std::cout << "Ready to play!" << std::endl;
    while (!CheckIsKeyDown('Q')) {
        for (auto& key : playableKeys) {
            bool isKeyDown = CheckIsKeyDown(key);
            bool didPlay = false;

            /* Emulate what the keyboard events would look like */
            if (isKeyDown && prevKeyState[key]) {
                didPlay = conductor.handleKeypressPressedEvent(key);
            } else if (isKeyDown) { // !prevKeyState[key]
                didPlay = conductor.handleKeypressDownEvent(key);
            } else if (prevKeyState[key]) { // !isKeyDown
                conductor.handleKeypressUpEvent(key);
                didPlay = true;
            }

            if (didPlay) prevKeyState[key] = isKeyDown;
        }
    }

    std::cout << "Playing Ends" << std::endl;
    conn.close();
    return 0;
}

int main(int argc, char** argv) {
    Library::setDeviceDbSource(DeviceDbSourceType::WEB_SERVICE, "https://api.zaber.io/device-db/master");
    // Library::setLogOutput(LogOutputMode::FILE, "zaboard.log");

    if (argc == 1) {
        return playLiveKeybaord();
    } else if (std::string(argv[1]) == "midilive") {
        return playLiveMidiBoard();
    } else {
        return playMidiFile(argv[1]);
    }
}
