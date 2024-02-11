#include "VirtualMouse.h"

VirtualMouse::VirtualMouse() {}

MouseState VirtualMouse::pop_state() {
  MouseState state_copy = state;
  reset_state();
  return state_copy;
}

void VirtualMouse::update_state(MouseState const* const new_state) {
  state.d_x = add_mouse_delta(state.d_x, new_state->d_x);
  state.d_y = add_mouse_delta(state.d_y, new_state->d_y);
  state.d_wheel = add_mouse_delta(state.d_wheel, new_state->d_wheel);

  for (uint8_t button = 0; button < MouseState::NUM_BUTTONS; button++) {
    state.buttons[button] |= new_state->buttons[button];
  }

  state.changed = new_state->changed;
}

void VirtualMouse::reset_state() {
  state.d_x = 0;
  state.d_y = 0;
  state.d_wheel = 0;
  memset(state.buttons, 0, MouseState::NUM_BUTTONS);
  state.changed = false;
}
