#include "notemap.h"
#include <utility>  // for pair
#include "csv.hpp"  // for with_column_content, with_column_name, CSVReader


/**** cNoteInfo *****/
cNoteInfo::cNoteInfo(): _midiNote(0), _noteName(""), _freqHz(0) {
}

cNoteInfo::cNoteInfo(int midiNote, std::string noteName, float freqHz, std::unordered_map<std::string, int>& deviceSpeedMap):
        _midiNote(midiNote),
        _noteName(noteName),
        _freqHz(freqHz),
        _deviceSpeedMap(deviceSpeedMap) {
}

int cNoteInfo::getMidiNote(){
    return _midiNote;
}

std::string cNoteInfo::getNoteName(){
    return _noteName;
}

float cNoteInfo::getFreq(){
    return _freqHz;
}

int cNoteInfo::getDeviceSpeed(std::string deviceName) {
    if (this->_deviceSpeedMap.count(deviceName)) {
        return this->_deviceSpeedMap[deviceName];

    // If it's not there, assume average of other speed.
    // Will result in 0 if no note info
    } else {
        int sum = 0;
        for (const auto& devSpeedRef: this->_deviceSpeedMap) {
            if (devSpeedRef.second != 0) sum += devSpeedRef.second;
        }
        return sum / this->_deviceSpeedMap.size();
    }
}


/**** cNoteMap *****/
cNoteMap cNoteMap::mapFromCSV(std::string fileName) {
    io::CSVReader<6> in(fileName);
    in.read_header(io::ignore_extra_column, "Midi Note", "Note Name", "Frequency (Hz)", "Speed VSR20", "Speed RST", "Speed RSW");

    cNoteMap noteMap;

    int midiNote; std::string noteName; float freqHz; /**/ int speedVSR; int speedRST; int speedRSW;
    while(in.read_row(midiNote, noteName, freqHz, speedVSR, speedRST, speedRSW)) {
        std::unordered_map<std::string, int> deviceSpeedMap = {
            {"VSR", speedVSR},
            {"RST", speedRST},
            {"RSW", speedRSW},
        };

        // cNoteInfo noteInfo (midiNote, noteName, freqHz, deviceSpeedMap);
        noteMap.add(cNoteInfo(midiNote, noteName, freqHz, deviceSpeedMap));
    }

    return noteMap;
}

void cNoteMap::add(cNoteInfo noteInfo) {
    this->_noteMap[noteInfo.getMidiNote()] = noteInfo;
}