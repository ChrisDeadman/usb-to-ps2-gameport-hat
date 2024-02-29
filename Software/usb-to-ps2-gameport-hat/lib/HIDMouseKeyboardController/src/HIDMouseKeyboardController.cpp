#include "HIDMouseKeyboardController.h"

HIDMouseKeyboardController::HIDMouseKeyboardController(HID *driver)
    : HIDKeyboardController::HIDKeyboardController(driver),
      HIDMouseController::HIDMouseController(driver) {
  driver->SetReportParser(0, (HIDReportParser *)this);
  driver->SetReportParser(1, (HIDReportParser *)this);
}

bool HIDMouseKeyboardController::is_connected() {
  return HIDKeyboardController::is_connected() || HIDMouseController::is_connected();
}

void HIDMouseKeyboardController::Parse(HID *hid, uint32_t is_rpt_id, uint32_t len, uint8_t *buf) {
  if (len < 6) {
    HIDMouseController::Parse(hid, is_rpt_id, len, buf);
  } else {
    HIDKeyboardController::Parse(hid, is_rpt_id, len, buf);
  }
};
