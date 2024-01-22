#ifndef HID_KEYBOARD_COMBINER_H
#define HID_KEYBOARD_COMBINER_H

#include <Arduino.h>
#include <hidboot.h>

#include "HIDKeyboardController.h"

class HIDKeyboardCombiner {
 private:
  static const uint8_t num_controllers = 2;
  HIDKeyboardController *controllers[num_controllers];

 public:
  HIDKeyboardCombiner(HIDKeyboardController *const controller_1,
                      HIDKeyboardController *const controller_2);

  /**
   * Returns whether a supported device is connected.
   */
  bool is_connected();

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
};

#endif  // HID_KEYBOARD_COMBINER_H
