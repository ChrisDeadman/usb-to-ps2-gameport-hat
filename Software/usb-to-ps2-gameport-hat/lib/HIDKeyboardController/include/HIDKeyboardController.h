#ifndef HID_KEYBOARD_CONTROLLER_H
#define HID_KEYBOARD_CONTROLLER_H

#include <Arduino.h>
#include <hidboot.h>

#include "KeyBuffer.h"
#include "KeyboardAction.h"
#include "KeyboardLeds.h"
#include "KeyboardModifierState.h"

class HIDKeyboardController : virtual public HIDReportParser {
 private:
  HID *driver;

  KeyboardModifierState modifier_state;
  uint8_t prev_state[KEYBOARD_KRO];
  KeyboardLeds led_state;
  KeyBuffer key_buffer;
  uint8_t send_report_buffer[1] __attribute__((aligned(4)));

 public:
  HIDKeyboardController(HID *driver);

  /**
   * Returns whether a supported device is connected.
   */
  virtual bool is_connected();

  /**
   * Returns the current modifier state.
   */
  KeyboardModifierState get_modifier_state();

  /**
   * Dequeues the next keyboard action.
   *
   * Returns `KbActionNone` if the queue is empty.
   */
  KeyboardAction deq();

  /**
   * Sets the state of the keyboard leds.
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
