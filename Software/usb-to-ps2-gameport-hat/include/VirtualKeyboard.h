#ifndef _VIRTUAL_KEYBOARD_H_
#define _VIRTUAL_KEYBOARD_H_

#include "KeyBuffer.h"
#include "KeyboardAction.h"
#include "KeyboardLeds.h"
#include "KeyboardModifierState.h"

class VirtualKeyboard {
 private:
  KeyboardModifierState modifier_state;
  KeyboardLeds led_state;
  KeyBuffer key_buffer;

 public:
  VirtualKeyboard();

  /**
   * Returns the current modifier state, then resets it.
   *
   * Deltas are summed up until pop_modifier_state is called.
   * This should be called periodically.
   */
  KeyboardModifierState pop_modifier_state();

  /**
   * Updates the state of the modifier keys.
   *
   * Deltas are summed up until pop_modifier_state is called.
   */
  void update_modifier_state(KeyboardModifierState new_state);

  /**
   * Returns the current LED state.
   */
  KeyboardLeds get_led_state();

  /**
   * Sets the state of the keyboard leds.
   */
  void set_led_state(KeyboardLeds new_state);

  /**
   * Dequeues the next keyboard action.
   *
   * Returns `KbActionNone` if the queue is empty.
   */
  KeyboardAction deq();

  /**
   * Enqueues a keyboard action.
   */
  void enq(KeyboardAction kb_action);
};

#endif  //_VIRTUAL_KEYBOARD_H_
