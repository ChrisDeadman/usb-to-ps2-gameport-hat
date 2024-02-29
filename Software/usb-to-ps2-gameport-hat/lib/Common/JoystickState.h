#ifndef _JOYSTICK_STATE_H_
#define _JOYSTICK_STATE_H_

#include <Arduino.h>

#define JOY_AXIS_CENTER 0x80

struct JoystickState {
  static const uint8_t NUM_AXES = 6;
  static const uint8_t NUM_BUTTONS = 10;

  uint8_t axes[NUM_AXES] = {JOY_AXIS_CENTER, JOY_AXIS_CENTER, JOY_AXIS_CENTER,
                            JOY_AXIS_CENTER, JOY_AXIS_CENTER, JOY_AXIS_CENTER};
  uint8_t buttons[NUM_BUTTONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  bool changed = false;
};

/**
 * Combines two axis.
 */
inline uint8_t combine_axes(uint8_t value_a, uint8_t value_b) {
  int16_t combined =
      ((int16_t)value_a - JOY_AXIS_CENTER) + ((int16_t)value_b - JOY_AXIS_CENTER) + JOY_AXIS_CENTER;
  if (combined < 0) combined = 0;
  if (combined > UINT8_MAX) combined = UINT8_MAX;
  return (uint8_t)combined;
}

#endif  // _JOYSTICK_STATE_H_
