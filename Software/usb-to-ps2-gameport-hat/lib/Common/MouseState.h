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

/**
 * Adds the delta to the value and prevents overflow.
 */
inline int16_t add_mouse_delta(int16_t value, int8_t delta) {
  int16_t result = value + delta;
  if (result > UINT8_MAX) {
    result = UINT8_MAX;
  } else if (result < -UINT8_MAX) {
    result = -UINT8_MAX;
  }
  return result;
}

#endif  // _MOUSE_STATE_H_
