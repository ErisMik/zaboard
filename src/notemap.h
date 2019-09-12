#pragma once

#include <string>
#include <unordered_map>

class cNoteInfo {
public:
    cNoteInfo();
    cNoteInfo(int midiNote, std::string noteName, float freqHz, std::unordered_map<std::string, int>& deviceSpeedMap);

    int getMidiNote();
    std::string getNoteName();
    float getFreq();
    int getDeviceSpeed(std::string deviceName);

private:
    int _midiNote;
    std::string _noteName;
    float _freqHz;
    std::unordered_map<std::string, int> _deviceSpeedMap;
};


class cNoteMap {
public:
    static cNoteMap mapFromCSV(std::string fileName = "NoteMap.csv");

    void add(cNoteInfo noteInfo);

private:
    std::unordered_map<int, cNoteInfo> _noteMap;
};