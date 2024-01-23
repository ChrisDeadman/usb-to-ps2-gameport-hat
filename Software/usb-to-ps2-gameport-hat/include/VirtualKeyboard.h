#ifndef _VIRTUAL_KEYBOARD_H_
#define _VIRTUAL_KEYBOARD_H_

#include "CircularBuffer.h"
#include "KeyboardCodes.h"
#include "KeyboardLeds.h"
#include "KeyboardModifierState.h"

#define VIRTUAL_KEYBOARD_KRO 6

class VirtualKeyboard {
 private:
  KeyboardModifierState modifier_state;
  KeyboardLeds led_state;
  CircularBuffer<KeyboardCodes, VIRTUAL_KEYBOARD_KRO> make_buffer;
  CircularBuffer<KeyboardCodes, VIRTUAL_KEYBOARD_KRO> brk_buffer;

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
   * Enqueues a make code.
   */
  void enq_make(KeyboardCodes code);

  /**
   * Enqueues a break code.
   */
  void enq_brk(KeyboardCodes code);
};

#endif  //_VIRTUAL_KEYBOARD_H_
