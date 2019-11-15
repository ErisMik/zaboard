#include <fcntl.h> // for open, O_NONBLOCK, O_RDONLY
#include <iostream> // for operator<<, endl, basic_ostream
#include <linux/input-event-codes.h> // for EV_KEY, KEY_A, KEY_CNT, KEY_D
#include <linux/input.h> // for input_event, EVIOCGNAME
#include <pthread.h> // for pthread_create, pthread_join
#include <sys/ioctl.h> // for ioctl
#include <unistd.h> // for close, read
#include <unordered_map> // for unordered_map

#define KEYBOARD_DEV "/dev/input/by-id/usb-DuckyChannel_International_Co.__Ltd._Ducky_Keyboard-event-kbd"

struct keyboard_state {
    signed short keys[KEY_CNT];
};

class cKeyboard {
private:
    pthread_t thread;
    bool active;
    int keyboard_fd;
    input_event* keyboard_ev;
    keyboard_state* keyboard_st;
    char name[256];

protected:
public:
    cKeyboard();
    ~cKeyboard();
    static void* loop(void* obj);
    void readEv();
    short getKeyState(short key);
};

cKeyboard::cKeyboard() {
    active = true;
    keyboard_fd = 0;
    keyboard_ev = new input_event();
    keyboard_st = new keyboard_state();
    keyboard_fd = open(KEYBOARD_DEV, O_RDONLY | O_NONBLOCK);
    if (keyboard_fd > 0) {
        ioctl(keyboard_fd, EVIOCGNAME(256), name);
        std::cout << "Keyboard up: " << name << std::endl;
        active = true;
        pthread_create(&thread, 0, &cKeyboard::loop, this);
    }
}

cKeyboard::~cKeyboard() {
    if (keyboard_fd > 0) {
        active = false;
        pthread_join(thread, 0);
        close(keyboard_fd);
    }
    delete keyboard_st;
    delete keyboard_ev;
    keyboard_fd = 0;
}

void* cKeyboard::loop(void* obj) {
    while (reinterpret_cast<cKeyboard*>(obj)->active) reinterpret_cast<cKeyboard*>(obj)->readEv();
    return nullptr;
}

void cKeyboard::readEv() {
    int bytes = read(keyboard_fd, keyboard_ev, sizeof(*keyboard_ev));

    if (bytes > 0) {
        if (keyboard_ev->type & EV_KEY) {
            keyboard_st->keys[keyboard_ev->code] = keyboard_ev->value;
        }
    }
}

short cKeyboard::getKeyState(short key) {
    return keyboard_st->keys[key];
}

bool CheckIsKeyDown(const char key) {
    static cKeyboard kb;
    static std::unordered_map<char, short> keymap {
        {'Q', KEY_Q},
        {'A', KEY_A},
        {'S', KEY_S},
        {'D', KEY_D},
        {'F', KEY_F},
        {'G', KEY_G},
        {'H', KEY_H},
        {'J', KEY_J},
        {'K', KEY_K},
        {'L', KEY_L},
        {'Z', KEY_Z},
        {'X', KEY_X},
        {'C', KEY_C},
        {'V', KEY_V},
        {'B', KEY_B},
        {'N', KEY_N},
        {'M', KEY_M}};

    return kb.getKeyState(keymap[key]) > 0;
}