#include "XBOXRECVMapper.h"

XBOXRECVMapper::XBOXRECVMapper(XBOXRECV* driver) : driver(driver) {}

uint8_t XBOXRECVMapper::getNumConnectedDevices() {
  uint8_t numConnected = 0;
  for (uint8_t controller = 0; controller < 4; controller++) {
    if (driver->Xbox360Connected[controller]) {
      ++numConnected;
    }
  }
  return numConnected;
}

uint8_t convertAxisValue(int16_t value) {
  if (value < 0) {
    return 0x80 + max((value / (-INT16_MIN / 0x80)), -0x80);
  } else {
    return 0x80 + min((value / (INT16_MAX / 0x7F)), 0x7F);
  }
}

JoystickState XBOXRECVMapper::getControllerState(uint8_t idx) {
  JoystickState newState;
  newState.versionCounter = state[idx].versionCounter + 1;

  // Analog axes
  newState.axes[0] = convertAxisValue(driver->getAnalogHat(LeftHatX, idx));
  newState.axes[1] = convertAxisValue(-driver->getAnalogHat(LeftHatY, idx));
  newState.axes[2] = convertAxisValue(driver->getAnalogHat(RightHatX, idx));
  newState.axes[3] = convertAxisValue(-driver->getAnalogHat(RightHatY, idx));

  // Digital axes
  if (driver->getButtonPress(LEFT, idx)) newState.axes[0] = 0;
  if (driver->getButtonPress(RIGHT, idx)) newState.axes[0] = UINT8_MAX;
  if (driver->getButtonPress(UP, idx)) newState.axes[1] = 0;
  if (driver->getButtonPress(DOWN, idx)) newState.axes[1] = UINT8_MAX;

  // Buttons
  newState.buttons[0] = driver->getButtonPress(A, idx) | driver->getButtonPress(R2, idx);
  newState.buttons[1] = driver->getButtonPress(B, idx) | driver->getButtonPress(L2, idx);
  newState.buttons[2] = driver->getButtonPress(X, idx);
  newState.buttons[3] = driver->getButtonPress(Y, idx);
  newState.buttons[4] = driver->getButtonPress(R1, idx);
  newState.buttons[5] = driver->getButtonPress(L1, idx);

  if (newState.axes[0] != state[idx].axes[0] ||
      newState.axes[1] != state[idx].axes[1] ||
      newState.axes[2] != state[idx].axes[2] ||
      newState.axes[3] != state[idx].axes[3] ||
      newState.buttons[0] != state[idx].buttons[0] ||
      newState.buttons[1] != state[idx].buttons[1] ||
      newState.buttons[2] != state[idx].buttons[2] ||
      newState.buttons[3] != state[idx].buttons[3] ||
      newState.buttons[4] != state[idx].buttons[4] ||
      newState.buttons[5] != state[idx].buttons[5]) {
    state[idx] = newState;
  }
  return state[idx];
}
