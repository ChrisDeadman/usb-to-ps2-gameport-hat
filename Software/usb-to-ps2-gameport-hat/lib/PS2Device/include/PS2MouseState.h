#ifndef PS2_MOUSE_STATE_H
#define PS2_MOUSE_STATE_H

#include <Arduino.h>

struct PS2MouseState {
  int8_t dX;
  int8_t dY;
  int8_t dWheel;
  bool button1;
  bool button2;
  bool button3;
  bool button4;
  bool button5;
};

#endif  // PS2_MOUSE_STATE_H
