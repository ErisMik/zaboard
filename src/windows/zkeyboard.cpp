#include <Windows.h>

bool CheckIsKeyDown(const char key) {
    return GetKeyState(key) & 0x8000;
}