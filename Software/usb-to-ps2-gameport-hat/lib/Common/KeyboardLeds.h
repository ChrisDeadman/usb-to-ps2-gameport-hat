#ifndef _KEYBOARD_LEDS_H_
#define _KEYBOARD_LEDS_H_

enum KeyboardLeds : uint8_t {
  KbLedNone = 0x0,
  KbLedScrollLock = 0x1,
  KbLedNumLock = 0x2,
  KbLedCapsLock = 0x4,
  KbLedAll = 0x7
};

#endif  //_KEYBOARD_LEDS_H_
