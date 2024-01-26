#include "VirtualMouse.h"

VirtualMouse::VirtualMouse() {}

MouseState VirtualMouse::pop_state() {
  MouseState state_copy = state;
  reset_state();
  return state_copy;
}

void VirtualMouse::update_state(MouseState const* const new_state) {
  state.d_x = combine_axis(state.d_x, new_state->d_x);
  state.d_y = combine_axis(state.d_y, new_state->d_y);
  state.d_wheel = combine_axis(state.d_wheel, new_state->d_wheel);
  memcpy(state.buttons, new_state->buttons, MouseState::NUM_BUTTONS);
  state.changed = new_state->changed;
}

void VirtualMouse::reset_state() {
  state.d_x = 0;
  state.d_y = 0;
  state.d_wheel = 0;
  memset(state.buttons, 0, MouseState::NUM_BUTTONS);
  state.changed = false;
}
