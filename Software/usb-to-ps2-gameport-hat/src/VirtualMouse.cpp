#include "VirtualMouse.h"

static int8_t combine_axis(int8_t value_a, int8_t value_b);

VirtualMouse::VirtualMouse() {}

MouseState VirtualMouse::pop_state() {
  MouseState state_copy = state;
  reset_state();
  return state_copy;
}

void VirtualMouse::update_state(MouseState const* const new_state) {
  state.d_x = combine_axis(state.d_x, new_state->d_x);
  // y is inverted
  state.d_y = combine_axis(state.d_y, -new_state->d_y);
  // wheel is inverted
  state.d_wheel = combine_axis(state.d_wheel, -new_state->d_wheel);
  state.button1 |= new_state->button1;
  state.button2 |= new_state->button2;
  state.button3 |= new_state->button3;
  state.button4 |= new_state->button4;
  state.button5 |= new_state->button5;
  state.changed = true;
}

void VirtualMouse::reset_state() {
  state.d_x = 0;
  state.d_y = 0;
  state.d_wheel = 0;
  state.button1 = false;
  state.button2 = false;
  state.button3 = false;
  state.button4 = false;
  state.button5 = false;
  state.changed = false;
}

static int8_t combine_axis(int8_t value_a, int8_t value_b) {
  int16_t combined = value_a + value_b;
  if (combined < INT8_MIN) {
    combined = INT8_MIN;
  } else if (combined > INT8_MAX) {
    combined = INT8_MAX;
  }
  return (int8_t)combined;
}
