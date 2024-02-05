#include "XBOXRECVMapper.h"

#define CONVERT_AXIS_VALUE(x) \
  (uint8_t)(((uint32_t)((x)-INT16_MIN) * UINT8_MAX) / (UINT16_MAX))

XBOXRECVMapper::XBOXRECVMapper(XBOXRECV* driver) : driver(driver) {}

uint8_t XBOXRECVMapper::get_num_connected_devices() {
  uint8_t num_connected = 0;
  for (uint8_t controller = 0; controller < 4; controller++) {
    if (driver->Xbox360Connected[controller]) {
      ++num_connected;
    }
  }
  return num_connected;
}

JoystickState XBOXRECVMapper::pop_state(uint8_t idx) {
  JoystickState old_state = state[idx];

  // Analog axes
  state[idx].axes[0] = CONVERT_AXIS_VALUE(driver->getAnalogHat(LeftHatX, idx));
  state[idx].axes[1] = CONVERT_AXIS_VALUE(-driver->getAnalogHat(LeftHatY, idx));
  state[idx].axes[2] = CONVERT_AXIS_VALUE(driver->getAnalogHat(RightHatX, idx));
  state[idx].axes[3] = CONVERT_AXIS_VALUE(-driver->getAnalogHat(RightHatY, idx));

  // Digital axes
  if (driver->getButtonPress(LEFT, idx)) state[idx].axes[0] = 0;
  if (driver->getButtonPress(RIGHT, idx)) state[idx].axes[0] = UINT8_MAX;
  if (driver->getButtonPress(UP, idx)) state[idx].axes[1] = 0;
  if (driver->getButtonPress(DOWN, idx)) state[idx].axes[1] = UINT8_MAX;

  // Buttons
  state[idx].buttons[0] =
      driver->getButtonPress(A, idx) | driver->getButtonPress(R2, idx);
  state[idx].buttons[1] =
      driver->getButtonPress(B, idx) | driver->getButtonPress(L2, idx);
  state[idx].buttons[2] = driver->getButtonPress(X, idx);
  state[idx].buttons[3] = driver->getButtonPress(Y, idx);
  state[idx].buttons[4] = driver->getButtonPress(R1, idx);
  state[idx].buttons[5] = driver->getButtonPress(L1, idx);
  state[idx].buttons[6] = driver->getButtonPress(SELECT, idx);
  state[idx].buttons[7] = driver->getButtonPress(START, idx);

  state[idx].changed = (state[idx].axes[0] != old_state.axes[0] ||
                        state[idx].axes[1] != old_state.axes[1] ||
                        state[idx].axes[2] != old_state.axes[2] ||
                        state[idx].axes[3] != old_state.axes[3] ||
                        state[idx].buttons[0] != old_state.buttons[0] ||
                        state[idx].buttons[1] != old_state.buttons[1] ||
                        state[idx].buttons[2] != old_state.buttons[2] ||
                        state[idx].buttons[3] != old_state.buttons[3] ||
                        state[idx].buttons[4] != old_state.buttons[4] ||
                        state[idx].buttons[5] != old_state.buttons[5] ||
                        state[idx].buttons[6] != old_state.buttons[6] ||
                        state[idx].buttons[7] != old_state.buttons[7]);
  return state[idx];
}
