#include "VirtualJoystick.h"

static uint8_t combine_axis(uint8_t value_a, uint8_t value_b);

VirtualJoystick::VirtualJoystick() {}

JoystickState VirtualJoystick::pop_state() {
  JoystickState state_copy = state;
  reset_state();
  return state_copy;
}

void VirtualJoystick::update_state(JoystickState const* const new_state,
                                   bool is_player_1, bool swap_joy_axis_3_and_4) {
  // PLAYER 1
  if (is_player_1) {
    state.buttons[0] |= new_state->buttons[0];
    state.buttons[1] |= new_state->buttons[1];
    state.buttons[2] |= new_state->buttons[2];
    state.buttons[3] |= new_state->buttons[3];
    state.buttons[4] |= new_state->buttons[4];
    state.buttons[5] |= new_state->buttons[5];
    state.buttons[6] |= new_state->buttons[6];
    state.buttons[7] |= new_state->buttons[7];
    state.axes[0] = combine_axis(state.axes[0], new_state->axes[0]);
    state.axes[1] = combine_axis(state.axes[1], new_state->axes[1]);
    if (swap_joy_axis_3_and_4) {
      state.axes[2] = combine_axis(state.axes[2], new_state->axes[3]);
      state.axes[3] = combine_axis(state.axes[3], new_state->axes[2]);
    } else {
      state.axes[2] = combine_axis(state.axes[2], new_state->axes[2]);
      state.axes[3] = combine_axis(state.axes[3], new_state->axes[3]);
    }
  }
  // PLAYER 2
  else {
    state.buttons[2] |= new_state->buttons[0];
    state.buttons[3] |= new_state->buttons[1];
    state.axes[2] = combine_axis(state.axes[2], new_state->axes[0]);
    state.axes[3] = combine_axis(state.axes[3], new_state->axes[1]);
  }

  state.changed = true;
}

void VirtualJoystick::reset_state() {
  memset(state.axes, 0x80, JoystickState::NUM_AXES);
  memset(state.buttons, 0, JoystickState::NUM_BUTTONS);
  state.changed = false;
}

static uint8_t combine_axis(uint8_t value_a, uint8_t value_b) {
  int16_t combined = ((int16_t)value_a - 0x80) + ((int16_t)value_b - 0x80) + 0x80;
  if (combined < 0) combined = 0;
  if (combined > UINT8_MAX) combined = UINT8_MAX;
  return (uint8_t)combined;
}
