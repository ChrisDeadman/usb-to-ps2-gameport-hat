#ifndef _JOYSTICK_LEDS_H_
#define _JOYSTICK_LEDS_H_

#include <Arduino.h>

enum JoystickLeds : uint8_t {
  JoyLedNone = 0x0,
  JoyLed1 = 0x1,
  JoyLed2 = 0x2
};

#endif  //_JOYSTICK_LEDS_H_
