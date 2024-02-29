#ifndef HID_JOYSTICK_CONTROLLER_H
#define HID_JOYSTICK_CONTROLLER_H

#include <hiduniversal.h>

#include "HIDDescriptionParser.h"
#include "HIDJoystickControllerState.h"

class HIDJoystickController : HIDUniversal, public HIDReportParser {
 private:
  USBHost *const usb;
  HIDDescriptionParser hidParser;
  HIDJoystickControllerState state;

 public:
  HIDJoystickController(USBHost *usb);

  /**
   * Returns whether a supported device is connected.
   */
  bool is_connected();

  /**
   * Returns the current controller state.
   */
  HIDJoystickControllerState pop_state();

  /**
   * HIDReportParser implementation.
   *
   * Called by the HIDUniversal driver whenever there are changes.
   */
  virtual void Parse(HID *hid, uint32_t is_rpt_id, uint32_t len, uint8_t *buf);

 private:
  /**
   * HIDUniversal implementation override so we can parse the device descriptor.
   */
  uint32_t Init(uint32_t parent, uint32_t port, uint32_t lowspeed) override;
};

#endif  // HID_JOYSTICK_CONTROLLER_H
