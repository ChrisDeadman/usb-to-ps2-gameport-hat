#include "HIDMouseController.h"

extern "C" {
void __usb_mouse_dummy_callback(uint8_t const *const data, uint8_t length) {}
}
/**
 * implement in your code if you want to capture packages.
 */
void usb_data_received(uint8_t const *const data, uint8_t length)
    __attribute__((weak, alias("__usb_mouse_dummy_callback")));

HIDMouseController::HIDMouseController(HID *driver) : driver(driver) {
  driver->SetReportParser(0, this);
  reset_state();
};

bool HIDMouseController::is_connected() { return driver->isReady(); }

MouseState HIDMouseController::pop_state() {
  MouseState state_copy = state;
  reset_state();
  return state_copy;
}

void HIDMouseController::Parse(HID * /* hid */, uint32_t /* is_rpt_id */,
                               uint32_t len, uint8_t *buf) {
  usb_data_received(buf, (uint8_t)len);

  if (len > 0) {
    state.buttons[0] = (buf[0] & 0x01) > 0;
    state.buttons[1] = (buf[0] & 0x02) > 0;
    state.buttons[2] = (buf[0] & 0x04) > 0;
    state.buttons[3] = (buf[0] & 0x08) > 0;
    state.buttons[4] = (buf[0] & 0x10) > 0;
    state.changed = true;
  }

  if (len > 2) {
    state.d_x = (int8_t)buf[1];
    state.d_y = (int8_t)buf[2];
  }

  if (len > 3) {
    state.d_wheel = (int8_t)buf[3];
  }
};

void HIDMouseController::reset_state() {
  state.d_x = 0;
  state.d_y = 0;
  state.d_wheel = 0;
  memset(state.buttons, 0, MouseState::NUM_BUTTONS);
  state.changed = false;
}
