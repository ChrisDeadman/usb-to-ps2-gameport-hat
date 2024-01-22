#ifndef _JOYSTICK_STATE_H_
#define _JOYSTICK_STATE_H_

#include <Arduino.h>

struct JoystickState {
  uint8_t version_counter = -1;  // overflow is expected, should just be checked
                                 // for equality.
  uint8_t axes[4] = {0x80, 0x80, 0x80, 0x80};
  bool buttons[6] = {0};
};

#endif  // _JOYSTICK_STATE_H_
