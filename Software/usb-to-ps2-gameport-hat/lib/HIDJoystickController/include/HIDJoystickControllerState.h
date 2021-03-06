#ifndef _HID_JOYSTICK_CONTROLLER_STATE_H_
#define _HID_JOYSTICK_CONTROLLER_STATE_H_

#include <Arduino.h>

struct HIDJoystickControllerState {
  static const uint8_t NUM_AXES = 8;
  static const uint8_t NUM_BUTTONS = 16;
  static const uint8_t NUM_HATS = 2;

  uint8_t versionCounter = -1;  // overflow is expected, should just be checked
                                // for equality.
  uint8_t axes[NUM_AXES] = {0};
  uint8_t buttons[NUM_BUTTONS] = {0};
  uint8_t hats[NUM_HATS] = {0};
};

#endif  // _HID_JOYSTICK_CONTROLLER_STATE_H_
