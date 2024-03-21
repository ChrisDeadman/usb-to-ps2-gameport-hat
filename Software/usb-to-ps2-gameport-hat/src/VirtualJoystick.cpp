#include <Arduino.h>

#include "VirtualJoystick.h"

VirtualJoystick::VirtualJoystick() {}

JoystickState VirtualJoystick::pop_state() {
  JoystickState state_copy = state;
  reset_state();
  return state_copy;
}

void VirtualJoystick::update_state(JoystickState const* const new_state, bool is_player_1,
                                   bool swap_joy_axis_3_and_4) {
  // PLAYER 1
  if (is_player_1) {
    state.axes[0] = combine_axes(state.axes[0], new_state->axes[0]);
    state.axes[1] = combine_axes(state.axes[1], new_state->axes[1]);
    if (swap_joy_axis_3_and_4) {
      state.axes[2] = combine_axes(state.axes[2], new_state->axes[3]);
      state.axes[3] = combine_axes(state.axes[3], new_state->axes[2]);
    } else {
      state.axes[2] = combine_axes(state.axes[2], new_state->axes[2]);
      state.axes[3] = combine_axes(state.axes[3], new_state->axes[3]);
    }
    state.axes[4] = combine_axes(state.axes[4], new_state->axes[4]);
    state.axes[5] = combine_axes(state.axes[5], new_state->axes[5]);
    for (uint8_t button = 0; button < JoystickState::NUM_BUTTONS; button++) {
      state.buttons[button] |= new_state->buttons[button];
    }
  }
  // PLAYER 2
  else {
    state.axes[2] = combine_axes(state.axes[2], new_state->axes[0]);
    state.axes[3] = combine_axes(state.axes[3], new_state->axes[1]);
    state.axes[4] = combine_axes(state.axes[4], new_state->axes[4]);
    state.axes[5] = combine_axes(state.axes[5], new_state->axes[5]);
    state.buttons[2] |= new_state->buttons[0] | new_state->buttons[2];
    state.buttons[3] |= new_state->buttons[1] | new_state->buttons[3];
    for (uint8_t button = 4; button < JoystickState::NUM_BUTTONS; button++) {
      state.buttons[button] |= new_state->buttons[button];
    }
  }

  state.changed = new_state->changed;
}

void VirtualJoystick::reset_state() {
  memset(state.axes, JOY_AXIS_CENTER, JoystickState::NUM_AXES);
  memset(state.buttons, 0, JoystickState::NUM_BUTTONS);
  state.changed = false;
}
