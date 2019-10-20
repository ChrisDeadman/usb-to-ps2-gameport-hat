#ifndef HID_MOUSE_CONTROLLER_H
#define HID_MOUSE_CONTROLLER_H

#include <Arduino.h>
#include <hidboot.h>
#include "HIDMouseControllerState.h"

class HIDMouseController : public HIDReportParser {
 private:
  HIDBoot<HID_PROTOCOL_MOUSE> hostMouse;
  HIDMouseControllerState state;

 public:
  HIDMouseController(USBHost *usb);

  /**
   * Returns the latest mouse state and resets it.
   *
   * Mouse deltas are summed up until popState is called.
   * This should be called periodically in the loop() function.
   */
  HIDMouseControllerState popState();

  /**
   * HIDReportParser implementation.
   *
   * Called by the HIDBoot driver whenever there are changes.
   */
  virtual void Parse(HID *hid, uint32_t is_rpt_id, uint32_t len, uint8_t *buf);

 private:
  void resetState();
};

#endif  // HID_MOUSE_CONTROLLER_H
