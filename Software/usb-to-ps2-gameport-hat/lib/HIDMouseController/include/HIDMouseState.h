#ifndef HID_MOUSE_CONTROLLER_STATE_H
#define HID_MOUSE_CONTROLLER_STATE_H

#include <Arduino.h>

struct HIDMouseState {
  uint8_t version_counter = -1;  // overflow is expected, should just be checked
                                 // for equality.
  int8_t d_x = 0;
  int8_t d_y = 0;
  int8_t d_wheel = 0;
  bool button1 = false;
  bool button2 = false;
  bool button3 = false;
  bool button4 = false;
  bool button5 = false;
};

#endif  // HID_MOUSE_CONTROLLER_STATE_H
