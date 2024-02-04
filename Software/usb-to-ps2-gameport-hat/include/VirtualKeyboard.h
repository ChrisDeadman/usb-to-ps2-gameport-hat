#ifndef _VIRTUAL_KEYBOARD_H_
#define _VIRTUAL_KEYBOARD_H_

#include "CircularBuffer.h"
#include "KeyboardAction.h"
#include "KeyboardLeds.h"
#include "KeyboardModifierState.h"

#define VIRTUAL_KEYBOARD_KRO 6

class VirtualKeyboard {
 private:
  KeyboardModifierState modifier_state;
  KeyboardLeds led_state;
  CircularBuffer<KeyboardAction, VIRTUAL_KEYBOARD_KRO * 2> action_buffer;

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
   * Returns the current LED state, then resets it.
   *
   * Deltas are summed up until pop_led_state is called.
   * This should be called periodically.
   */
  KeyboardLeds pop_led_state();

  /**
   * Dequeues the next keyboard action.
   *
   * Returns `KbActionNone` if the queue is empty.
   */
  KeyboardAction deq();

  /**
   * Updates the state of the keyboard leds.
   *
   * Deltas are summed up until pop_modifier_state is called.
   */
  void update_modifier_state(KeyboardModifierState new_state);

  /**
   * Updates the state of the keyboard leds.
   *
   * Deltas are summed up until pop_led_state is called.
   */
  void update_led_state(KeyboardLeds new_state);

  /**
   * Enqueues a keyboard action.
   */
  void enq(KeyboardAction kb_action);
};

#endif  //_VIRTUAL_KEYBOARD_H_
