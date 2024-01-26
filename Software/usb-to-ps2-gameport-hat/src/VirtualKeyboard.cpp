#include "VirtualKeyboard.h"

VirtualKeyboard::VirtualKeyboard() {}

KeyboardModifierState VirtualKeyboard::pop_modifier_state() {
  KeyboardModifierState state_copy = modifier_state;
  modifier_state = ModNone;
  return state_copy;
}

KeyboardLeds VirtualKeyboard::pop_led_state() {
  KeyboardLeds state_copy = led_state;
  led_state = LedNone;
  return state_copy;
}

KeyboardCodes VirtualKeyboard::deq_make() {
  if (make_buffer.length() <= 0) {
    return NoKey;
  }
  return make_buffer.deq();
}

KeyboardCodes VirtualKeyboard::deq_brk() {
  if (brk_buffer.length() <= 0) {
    return NoKey;
  }
  return brk_buffer.deq();
}

void VirtualKeyboard::update_modifier_state(KeyboardModifierState new_state) {
  modifier_state = (KeyboardModifierState)(modifier_state | new_state);
}

void VirtualKeyboard::update_led_state(KeyboardLeds new_state) {
  led_state = (KeyboardLeds)(led_state | new_state);
}

void VirtualKeyboard::enq_make(KeyboardCodes code) { make_buffer.enq(code); }

void VirtualKeyboard::enq_brk(KeyboardCodes code) { brk_buffer.enq(code); }
