#include <Arduino.h>

#include "HIDKeyboardController.h"

extern "C" {
void __usb_kbd_dummy_callback(uint8_t const *const data, uint8_t length) {}
}
/**
 * implement in your code if you want to capture packages.
 */
void usb_data_received(uint8_t const *const data, uint8_t length)
    __attribute__((weak, alias("__usb_kbd_dummy_callback")));

void usb_data_sent(uint8_t const *const data, uint8_t length)
    __attribute__((weak, alias("__usb_kbd_dummy_callback")));

HIDKeyboardController::HIDKeyboardController(HID *driver) : driver(driver) {
  driver->SetReportParser(0, this);
  led_state = KbLedNone;
}

bool HIDKeyboardController::is_connected() { return driver->isReady(); }

KeyboardModifierState HIDKeyboardController::get_modifier_state() {
  return kb_parser.get_modifier_state();
}

KeyboardAction HIDKeyboardController::deq() { return kb_parser.deq(); }

void HIDKeyboardController::set_led_state(KeyboardLeds new_state) {
  if (!is_connected() || new_state == led_state) {
    return;
  }
  led_state = new_state;

  send_report_buffer[0] = 0;
  if (led_state & KbLedNumLock) send_report_buffer[0] |= 0x1;
  if (led_state & KbLedCapsLock) send_report_buffer[0] |= 0x2;
  if (led_state & KbLedScrollLock) send_report_buffer[0] |= 0x4;

  driver->SetReport(0, 0, 2, 0, 1, send_report_buffer);
  usb_data_sent(send_report_buffer, 1);
}

void HIDKeyboardController::Parse(HID * /* hid */,
                                  uint32_t /* is_rpt_id */,
                                  uint32_t len,
                                  uint8_t *buf) {
  usb_data_received(buf, (uint8_t)len);

  kb_parser.update(len, buf);
}
