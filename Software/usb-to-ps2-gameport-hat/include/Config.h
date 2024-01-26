#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "KeyboardCodes.h"

#define Serial Serial1  // use pins D0/D1
#define SERIAL_SPEED 115200

#define DEBUG  // print sent/received data

#define PS2_1_DATA_PIN 2
#define PS2_1_CLOCK_PIN 3

#define PS2_2_DATA_PIN 17
#define PS2_2_CLOCK_PIN 16

#define JOY_BUTTON1_PIN 4
#define JOY_BUTTON2_PIN 5
#define JOY_BUTTON3_PIN 6
#define JOY_BUTTON4_PIN 7

#define EXT_LED1_PIN 8
#define EXT_LED2_PIN 9

#define POT1_CS_PIN 10

#define SETUP_ENTER_DELAY 1000
#define SETUP_BLINK_WINDOW 1500

#define MOUSE_EMU_SPEED 10

#define NUM_KB_EMU_MAPPINGS 16

static const KeyboardCodes KB_EMU_MAPPINGS[NUM_KB_EMU_MAPPINGS][2][2] = {
    {{LeftArrow, NoKey}, {NoKey, NoKey}},
    {{RightArrow, NoKey}, {NoKey, NoKey}},
    {{UpArrow, NoKey}, {NoKey, NoKey}},
    {{DownArrow, NoKey}, {NoKey, NoKey}},
    {{Keypad4Left, NoKey}, {NoKey, NoKey}},
    {{Keypad6Right, NoKey}, {NoKey, NoKey}},
    {{Keypad8Up, NoKey}, {NoKey, NoKey}},
    {{Keypad2Down, NoKey}, {NoKey, NoKey}},
    {{LeftControl, NoKey}, {RightControl, NoKey}},
    {{Space, NoKey}, {RightShift, NoKey}},
    {{LeftAlt, NoKey}, {CapsLock, NoKey}},
    {{LeftShift, NoKey}, {NoKey, NoKey}},
    {{LeftAlt, RightArrow}, {NoKey, NoKey}},
    {{LeftAlt, LeftArrow}, {NoKey, NoKey}},
    {{Escape, NoKey}, {NoKey, NoKey}},
    {{Tab, NoKey}, {NoKey, NoKey}},
};

#endif  // _CONFIG_H_
