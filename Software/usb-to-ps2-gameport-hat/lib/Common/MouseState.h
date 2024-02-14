#ifndef _MOUSE_STATE_H_
#define _MOUSE_STATE_H_

#include <Arduino.h>

struct MouseState {
  static const uint8_t NUM_BUTTONS = 5;

  int16_t d_x = 0;
  int16_t d_y = 0;
  int16_t d_wheel = 0;
  uint8_t buttons[NUM_BUTTONS] = {0, 0, 0, 0, 0};
  bool changed = false;
};

#endif  // _MOUSE_STATE_H_
