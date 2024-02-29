#ifndef HID_MOUSE_KEYBOARD_CONTROLLER_H
#define HID_MOUSE_KEYBOARD_CONTROLLER_H

#include <Arduino.h>
#include <hidboot.h>

#include "HIDKeyboardController.h"
#include "HIDMouseController.h"

class HIDMouseKeyboardController : public HIDKeyboardController, public HIDMouseController {
 public:
  HIDMouseKeyboardController(HID *driver);

  /**
   * Returns whether a supported device is connected.
   */
  virtual bool is_connected();

  /**
   * HIDReportParser implementation.
   *
   * Called by the HID driver whenever there are changes.
   */
  virtual void Parse(HID *hid, uint32_t is_rpt_id, uint32_t len, uint8_t *buf);
};

#endif  // HID_MOUSE_KEYBOARD_CONTROLLER_H
