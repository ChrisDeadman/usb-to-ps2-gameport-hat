#ifndef _MOUSE_STATE_H_
#define _MOUSE_STATE_H_

#include <Arduino.h>

struct MouseState {
  int8_t d_x = 0;
  int8_t d_y = 0;
  int8_t d_wheel = 0;
  bool button1 = false;
  bool button2 = false;
  bool button3 = false;
  bool button4 = false;
  bool button5 = false;
  bool changed = false;
};

#endif  // _MOUSE_STATE_H_
