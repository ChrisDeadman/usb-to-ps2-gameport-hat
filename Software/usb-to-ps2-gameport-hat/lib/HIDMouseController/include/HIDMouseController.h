#ifndef HID_MOUSE_CONTROLLER_H
#define HID_MOUSE_CONTROLLER_H

#include <Arduino.h>
#include <hidboot.h>

#include "MouseState.h"

class HIDMouseController : virtual public HIDReportParser {
 private:
  HID *driver;

  MouseState state;

 public:
  HIDMouseController(HID *driver);

  /**
   * Returns whether a supported device is connected.
   */
  virtual bool is_connected();

  /**
   * Returns the current state, then resets it.
   *
   * Deltas are summed up until pop_state is called.
   * This should be called periodically.
   */
  MouseState pop_state();

  /**
   * HIDReportParser implementation.
   *
   * Called by the HID driver whenever there are changes.
   */
  virtual void Parse(HID *hid, uint32_t is_rpt_id, uint32_t len, uint8_t *buf);

 private:
  void reset_state();
};

#endif  // HID_MOUSE_CONTROLLER_H
