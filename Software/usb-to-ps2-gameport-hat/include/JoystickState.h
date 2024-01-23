#ifndef _JOYSTICK_STATE_H_
#define _JOYSTICK_STATE_H_

#include <Arduino.h>

struct JoystickState {
  static const uint8_t NUM_AXES = 4;
  static const uint8_t NUM_BUTTONS = 6;

  uint8_t axes[NUM_AXES] = {0x80, 0x80, 0x80, 0x80};
  uint8_t buttons[NUM_BUTTONS] = {0, 0, 0, 0, 0, 0};

  bool changed = false;
};

#endif  // _JOYSTICK_STATE_H_
