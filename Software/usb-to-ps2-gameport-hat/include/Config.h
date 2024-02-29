#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "KeyboardCodes.h"

#define Serial Serial1  // use pins D0/D1
#define SERIAL_SPEED 115200

// #define DEBUG_USB
#define DEBUG_PS2

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

#define JOY_AXIS_TEST(axis, positive)                \
  ((positive) ? ((axis) >= (JOY_AXIS_CENTER + 0x30)) \
              : ((axis) <= (JOY_AXIS_CENTER - 0x30)))

#define XBOX_LED_FLASH_INTERVAL 10

#define MOUSE_EMU_SPEED 5

#define NUM_KB_EMU_MAPPINGS 16

// clang-format off
static const KeyboardCodes KB_EMU_MAPPINGS[NUM_KB_EMU_MAPPINGS][2] = {
    {LeftArrow,     Keypad4Left},
    {RightArrow,    Keypad6Right},
    {UpArrow,       Keypad8Up},
    {DownArrow,     Keypad2Down},
    {aA,            NoKey},
    {dD,            NoKey},
    {wW,            NoKey},
    {sS,            NoKey},
    {LeftControl,   RightControl},
    {Space,         RightShift},
    {LeftAlt,       CapsLock},
    {LeftShift,     NoKey},
    {PeriodGreater, NoKey},
    {CommaLess,     NoKey},
    {Escape,        NoKey},
    {Tab,           NoKey},
};
// clang-format on

#endif  // _CONFIG_H_
