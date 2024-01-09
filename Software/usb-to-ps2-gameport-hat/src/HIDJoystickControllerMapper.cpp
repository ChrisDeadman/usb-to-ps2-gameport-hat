#include "HIDJoystickControllerMapper.h"

HIDJoystickControllerMapper::HIDJoystickControllerMapper(
    HIDJoystickController* driver)
    : driver(driver) {}

uint8_t HIDJoystickControllerMapper::getNumConnectedDevices() {
  return driver->is_connected() ? 1 : 0;
}

JoystickState HIDJoystickControllerMapper::getControllerState(
    uint8_t /* idx */) {
  static const uint8_t NUM_AXES = HIDJoystickControllerState::NUM_AXES;
  static const uint8_t NUM_BUTTONS = HIDJoystickControllerState::NUM_BUTTONS;

  HIDJoystickControllerState driverState = driver->get_state();

  if (driverState.version_counter != state.version_counter) {
    state.version_counter = driverState.version_counter;

    // Analog axes
    state.axes[0] = driverState.axes[0 % NUM_AXES];
    state.axes[1] = driverState.axes[1 % NUM_AXES];
    state.axes[2] = driverState.axes[2 % NUM_AXES];
    state.axes[3] = driverState.axes[3 % NUM_AXES];

    // Digital axes
    if ((driverState.hats[0] >= 1) && (driverState.hats[0] <= 3))  // LEFT
      state.axes[0] = UINT8_MAX;
    if ((driverState.hats[0] >= 5) && (driverState.hats[0] <= 7))  // RIGHT
      state.axes[0] = 0;
    if ((driverState.hats[0] >= 3) && (driverState.hats[0] <= 5))  // UP
      state.axes[1] = UINT8_MAX;
    if ((driverState.hats[0] == 7) || (driverState.hats[0] <= 1))  // DOWN
      state.axes[1] = 0;

    // Buttons
    state.buttons[0] = driverState.buttons[0 % NUM_BUTTONS];
    state.buttons[1] = driverState.buttons[1 % NUM_BUTTONS];
    state.buttons[2] = driverState.buttons[2 % NUM_BUTTONS];
    state.buttons[3] = driverState.buttons[3 % NUM_BUTTONS];
    state.buttons[4] = driverState.buttons[4 % NUM_BUTTONS];
    state.buttons[5] = driverState.buttons[5 % NUM_BUTTONS];
  }

  return state;
}
