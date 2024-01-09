#ifndef PS2_MOUSE_STATE_H
#define PS2_MOUSE_STATE_H

#include <Arduino.h>

struct PS2MouseState {
  int8_t d_x;
  int8_t d_y;
  int8_t d_wheel;
  bool button1;
  bool button2;
  bool button3;
  bool button4;
  bool button5;
};

#endif  // PS2_MOUSE_STATE_H
