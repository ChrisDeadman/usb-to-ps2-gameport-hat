#ifndef HID_MOUSE_CONTROLLER_H
#define HID_MOUSE_CONTROLLER_H

#include <Arduino.h>
#include <hidboot.h>

#include "HIDMouseState.h"

class HIDMouseController : virtual public HIDReportParser {
 private:
  HID *driver;

  HIDMouseState state;

 public:
  HIDMouseController(HID *driver);

  /**
   * Returns whether a supported device is connected.
   */
  virtual bool is_connected();

  /**
   * Returns the latest mouse state and resets it.
   *
   * Mouse deltas are summed up until popState is called.
   * This should be called periodically.
   */
  HIDMouseState pop_state();

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
