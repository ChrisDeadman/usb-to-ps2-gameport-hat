#include "HIDJoystickControllerMapper.h"

HIDJoystickControllerMapper::HIDJoystickControllerMapper(
    HIDJoystickController* driver)
    : driver(driver) {}

uint8_t HIDJoystickControllerMapper::get_num_connected_devices() {
  return driver->is_connected() ? 1 : 0;
}

JoystickState HIDJoystickControllerMapper::pop_state(uint8_t /* idx */) {
  static const uint8_t NUM_AXES = HIDJoystickControllerState::NUM_AXES;
  static const uint8_t NUM_BUTTONS = HIDJoystickControllerState::NUM_BUTTONS;

  JoystickState state;
  HIDJoystickControllerState driver_state = driver->pop_state();

  // Analog axes
  state.axes[0] = driver_state.axes[0 % NUM_AXES];
  state.axes[1] = driver_state.axes[1 % NUM_AXES];
  state.axes[2] = driver_state.axes[2 % NUM_AXES];
  state.axes[3] = driver_state.axes[3 % NUM_AXES];

  // Digital axes
  if ((driver_state.hats[0] >= 1) && (driver_state.hats[0] <= 3))  // LEFT
    state.axes[0] = UINT8_MAX;
  if ((driver_state.hats[0] >= 5) && (driver_state.hats[0] <= 7))  // RIGHT
    state.axes[0] = 0;
  if ((driver_state.hats[0] >= 3) && (driver_state.hats[0] <= 5))  // UP
    state.axes[1] = UINT8_MAX;
  if ((driver_state.hats[0] == 7) || (driver_state.hats[0] <= 1))  // DOWN
    state.axes[1] = 0;

  // Buttons
  state.buttons[0] = driver_state.buttons[0 % NUM_BUTTONS];
  state.buttons[1] = driver_state.buttons[1 % NUM_BUTTONS];
  state.buttons[2] = driver_state.buttons[2 % NUM_BUTTONS];
  state.buttons[3] = driver_state.buttons[3 % NUM_BUTTONS];
  state.buttons[4] = driver_state.buttons[4 % NUM_BUTTONS];
  state.buttons[5] = driver_state.buttons[5 % NUM_BUTTONS];
  state.buttons[6] = driver_state.buttons[6 % NUM_BUTTONS];
  state.buttons[7] = driver_state.buttons[7 % NUM_BUTTONS];

  state.changed = driver_state.changed;

  return state;
}
