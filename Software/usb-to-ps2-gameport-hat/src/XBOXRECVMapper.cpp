#include "XBOXRECVMapper.h"

#include "Config.h"

#define CONVERT_AXIS_VALUE(x) (uint8_t)(((uint32_t)((x)-INT16_MIN) * UINT8_MAX) / (UINT16_MAX))

XBOXRECVMapper::XBOXRECVMapper(XBOXRECV* driver)
    : driver(driver), led_state(JoyLedDefault), current_led_idx(0) {}

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
  state[idx].axes[4] = JOY_AXIS_CENTER;
  state[idx].axes[5] = JOY_AXIS_CENTER;
  if (driver->getButtonPress(LEFT, idx)) state[idx].axes[4] = 0;
  if (driver->getButtonPress(RIGHT, idx)) state[idx].axes[4] = UINT8_MAX;
  if (driver->getButtonPress(UP, idx)) state[idx].axes[5] = 0;
  if (driver->getButtonPress(DOWN, idx)) state[idx].axes[5] = UINT8_MAX;

  // Buttons
  state[idx].buttons[0] = driver->getButtonPress(A, idx);
  state[idx].buttons[1] = driver->getButtonPress(B, idx);
  state[idx].buttons[2] = driver->getButtonPress(X, idx);
  state[idx].buttons[3] = driver->getButtonPress(Y, idx);
  state[idx].buttons[4] = driver->getButtonPress(R1, idx);
  state[idx].buttons[5] = driver->getButtonPress(L1, idx);
  state[idx].buttons[6] = driver->getButtonPress(R2, idx);
  state[idx].buttons[7] = driver->getButtonPress(L2, idx);
  state[idx].buttons[8] = driver->getButtonPress(SELECT, idx);
  state[idx].buttons[9] = driver->getButtonPress(START, idx);

  state[idx].changed = false;
  for (uint8_t axis = 0; axis < JoystickState::NUM_AXES; axis++) {
    if (state[idx].axes[axis] != old_state.axes[axis]) {
      state[idx].changed = true;
      break;
    }
  }
  for (uint8_t button = 0; button < JoystickState::NUM_BUTTONS; button++) {
    if (state[idx].buttons[button] != old_state.buttons[button]) {
      state[idx].changed = true;
      break;
    }
  }
  return state[idx];
}

void XBOXRECVMapper::set_led_state(JoystickLeds new_state) {
  if (new_state == led_state) {
    return;
  }

  if (new_state == JoyLedDefault) {
    driver->setLedOn(LED1);
    led_state = JoyLedDefault;
  } else if ((current_led_idx == 0) && (new_state & JoyLed1)) {
    driver->setLedOn(LED1);
    led_state = JoyLed1;
  } else if ((current_led_idx == 1) && (new_state & JoyLed2)) {
    driver->setLedOn(LED3);
    led_state = JoyLed2;
  } else {
    driver->setLedOff();
    led_state = JoyLedNone;
  }
  current_led_idx += 1;
  if (current_led_idx > 1) {
    current_led_idx = 0;
  }
}
