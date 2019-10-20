#include "HIDMouseController.h"

inline int8_t addDelta(int8_t value, int8_t delta) {
  int16_t result = (int16_t)value + delta;
  if (result > 128) {
    return 128;
  } else if (result < -127) {
    return -127;
  } else {
    return (int8_t)(value + delta);
  }
}

HIDMouseController::HIDMouseController(USBHost *usb) : hostMouse(usb) {
  hostMouse.SetReportParser(0, this);
  resetState();
};

HIDMouseControllerState HIDMouseController::popState() {
  HIDMouseControllerState stateCopy;
  stateCopy.versionCounter = state.versionCounter;
  stateCopy.dX = state.dX;
  stateCopy.dY = state.dY;
  stateCopy.dWheel = state.dWheel;
  stateCopy.button1 = state.button1;
  stateCopy.button2 = state.button2;
  stateCopy.button3 = state.button3;
  stateCopy.button4 = state.button4;
  stateCopy.button5 = state.button5;
  resetState();
  return stateCopy;
}

void HIDMouseController::Parse(HID * /* hid */, uint32_t /* is_rpt_id */,
                               uint32_t len, uint8_t *buf) {
  if (len > 0) {
    state.versionCounter += 1;
    state.button1 = (buf[0] & 0x01) > 0;
    state.button2 = (buf[0] & 0x02) > 0;
    state.button3 = (buf[0] & 0x04) > 0;
    state.button4 = (buf[0] & 0x08) > 0;
    state.button5 = (buf[0] & 0x10) > 0;
  }

  if (len > 2) {
    state.dX = addDelta(state.dX, buf[1]);
    state.dY = addDelta(state.dY, buf[2]);
  }

  if (len > 3) {
    state.dWheel = addDelta(state.dWheel, buf[3]);
  }
};

void HIDMouseController::resetState() {
  state.dX = 0;
  state.dY = 0;
  state.dWheel = 0;
  state.button1 = 0;
  state.button2 = 0;
  state.button3 = 0;
  state.button4 = 0;
  state.button5 = 0;
}
