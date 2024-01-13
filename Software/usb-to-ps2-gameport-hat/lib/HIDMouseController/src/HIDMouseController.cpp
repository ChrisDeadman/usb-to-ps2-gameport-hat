#include "HIDMouseController.h"

static int8_t add_delta(int8_t value, int8_t delta);

HIDMouseController::HIDMouseController(HID *driver) : driver(driver) {
  driver->SetReportParser(0, this);
  connected = false;
  reset_state();
};

bool HIDMouseController::is_connected() { return connected; }

HIDMouseState HIDMouseController::pop_state() {
  HIDMouseState stateCopy;
  stateCopy.version_counter = state.version_counter;
  stateCopy.d_x = state.d_x;
  stateCopy.d_y = state.d_y;
  stateCopy.d_wheel = state.d_wheel;
  stateCopy.button1 = state.button1;
  stateCopy.button2 = state.button2;
  stateCopy.button3 = state.button3;
  stateCopy.button4 = state.button4;
  stateCopy.button5 = state.button5;
  reset_state();
  return stateCopy;
}

void HIDMouseController::Parse(HID * /* hid */, uint32_t /* is_rpt_id */,
                               uint32_t len, uint8_t *buf) {
  connected = true;

  if (len > 0) {
    state.version_counter += 1;
    state.button1 = (buf[0] & 0x01) > 0;
    state.button2 = (buf[0] & 0x02) > 0;
    state.button3 = (buf[0] & 0x04) > 0;
    state.button4 = (buf[0] & 0x08) > 0;
    state.button5 = (buf[0] & 0x10) > 0;
  }

  if (len > 2) {
    state.d_x = add_delta(state.d_x, buf[1]);
    state.d_y = add_delta(state.d_y, buf[2]);
  }

  if (len > 3) {
    state.d_wheel = add_delta(state.d_wheel, buf[3]);
  }
};

void HIDMouseController::reset_state() {
  state.d_x = 0;
  state.d_y = 0;
  state.d_wheel = 0;
  state.button1 = 0;
  state.button2 = 0;
  state.button3 = 0;
  state.button4 = 0;
  state.button5 = 0;
}

static int8_t add_delta(int8_t value, int8_t delta) {
  int16_t result = (int16_t)value + delta;
  if (result > 128) {
    return 128;
  } else if (result < -127) {
    return -127;
  } else {
    return (int8_t)(value + delta);
  }
}