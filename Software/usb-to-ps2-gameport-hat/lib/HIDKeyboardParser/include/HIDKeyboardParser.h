#ifndef HID_KEYBOARD_PARSER_H
#define HID_KEYBOARD_PARSER_H

#include "KeyBuffer.h"
#include "KeyboardAction.h"
#include "KeyboardLeds.h"
#include "KeyboardModifierState.h"

class HIDKeyboardParser {
 private:
  KeyboardModifierState modifier_state;
  KeyboardCodes key_state[KEYBOARD_KRO];
  uint32_t key_state_len;
  KeyBuffer key_buffer;

 public:
  HIDKeyboardParser();

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
   * Updates the current state according to received keyboard data.
   */
  void update(uint32_t len, uint8_t *buf);
};

#endif  // HID_KEYBOARD_PARSER_H
