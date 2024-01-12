#ifndef _KEYBOARD_LEDS_H_
#define _KEYBOARD_LEDS_H_

#include <Arduino.h>

enum KeyboardLeds : uint8_t {
  LedNone = 0x0,
  LedScrollLock = 0x1,
  LedNumLock = 0x2,
  LedCapsLock = 0x4,
  LedAll = 0x7
};

#endif  //_KEYBOARD_LEDS_H_
