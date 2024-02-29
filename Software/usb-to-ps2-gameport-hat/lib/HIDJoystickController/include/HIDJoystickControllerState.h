#ifndef _HID_JOYSTICK_CONTROLLER_STATE_H_
#define _HID_JOYSTICK_CONTROLLER_STATE_H_

#include <Arduino.h>

#include "JoystickState.h"

struct HIDJoystickControllerState {
  static const uint8_t NUM_AXES = 8;
  static const uint8_t NUM_BUTTONS = 16;
  static const uint8_t NUM_HATS = 2;

  uint8_t axes[NUM_AXES] = {JOY_AXIS_CENTER, JOY_AXIS_CENTER, JOY_AXIS_CENTER, JOY_AXIS_CENTER,
                            JOY_AXIS_CENTER, JOY_AXIS_CENTER, JOY_AXIS_CENTER, JOY_AXIS_CENTER};
  uint8_t buttons[NUM_BUTTONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t hats[NUM_HATS] = {0, 0};

  bool changed = false;
};

#endif  // _HID_JOYSTICK_CONTROLLER_STATE_H_
