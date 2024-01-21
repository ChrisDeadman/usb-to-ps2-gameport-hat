#ifndef HID_KEYBOARD_CONTROLLER_H
#define HID_KEYBOARD_CONTROLLER_H

#include <Arduino.h>
#include <hidboot.h>

#include "CircularBuffer.h"
#include "KeyboardCodes.h"
#include "KeyboardLeds.h"

#define USB_KEYBOARD_KRO 6

enum ModifierState : uint8_t {
  ModNone = 0,
  ModLeftCtrl = 0x01,
  ModLeftShift = 0x02,
  ModLeftAlt = 0x04,
  ModLeftGUI = 0x08,
  ModRightCtrl = 0x10,
  ModRightShift = 0x20,
  ModRightAlt = 0x40,
  ModRightGUI = 0x80,
};

class HIDKeyboardController : virtual public HIDReportParser {
 private:
  HID *driver;

  ModifierState modifier_state;
  uint8_t prev_state[USB_KEYBOARD_KRO];
  KeyboardLeds led_state;
  CircularBuffer<KeyboardCodes, USB_KEYBOARD_KRO> make_buffer;
  CircularBuffer<KeyboardCodes, USB_KEYBOARD_KRO> brk_buffer;

 public:
  HIDKeyboardController(HID *driver);

  /**
   * Returns whether a supported device is connected.
   */
  virtual bool is_connected();

  /**
   * Returns the current modifier state.
   */
  ModifierState get_modifier_state();

  /**
   * Dequeues the next make code.
   *
   * Returns `KeyboardCodes::NoKey` if the queue is empty.
   */
  KeyboardCodes deq_make();

  /**
   * Dequeues the next break code.
   *
   * Returns `KeyboardCodes::NoKey` if the queue is empty.
   */
  KeyboardCodes deq_brk();

  /**
   * Updates the state of the keyboard leds.
   */
  void set_led_state(KeyboardLeds state);

  /**
   * HIDReportParser implementation.
   *
   * Called by the HID driver whenever there are changes.
   */
  virtual void Parse(HID *hid, uint32_t is_rpt_id, uint32_t len, uint8_t *buf);
};

#endif  // HID_KEYBOARD_CONTROLLER_H
