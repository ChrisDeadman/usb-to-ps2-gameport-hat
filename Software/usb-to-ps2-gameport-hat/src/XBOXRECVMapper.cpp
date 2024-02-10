#include "XBOXRECVMapper.h"

#define CONVERT_AXIS_VALUE(x) \
  (uint8_t)(((uint32_t)((x)-INT16_MIN) * UINT8_MAX) / (UINT16_MAX))

XBOXRECVMapper::XBOXRECVMapper(XBOXRECV* driver)
    : driver(driver), led_state(JoyLedNone), current_led_idx(0) {}

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

  // Update LED state
  update_led_state(idx);

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

void XBOXRECVMapper::set_led_state(JoystickLeds new_state) {
  // restore default LED state
  if ((new_state != led_state) && (new_state == JoyLedNone)) {
    driver->setLedOn(LED1);
  }
  led_state = new_state;
}

void XBOXRECVMapper::update_led_state(uint8_t idx) {
  // Only set LEDs on controller 0
  if (idx != 0) {
    return;
  }

  if ((current_led_idx == 0) && (led_state & JoyLed1)) {
    driver->setLedOn(LED3);
  }
  if ((current_led_idx == 1) && (led_state & JoyLed2)) {
    driver->setLedOn(LED4);
  }
  current_led_idx += 1;
  if (current_led_idx > 1) {
    current_led_idx = 0;
  }
}
