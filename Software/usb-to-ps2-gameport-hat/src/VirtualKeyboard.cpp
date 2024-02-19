#include "VirtualKeyboard.h"

VirtualKeyboard::VirtualKeyboard() {}

KeyboardModifierState VirtualKeyboard::pop_modifier_state() {
  KeyboardModifierState state_copy = modifier_state;
  modifier_state = ModNone;
  return state_copy;
}

void VirtualKeyboard::update_modifier_state(KeyboardModifierState new_state) {
  modifier_state = (KeyboardModifierState)(modifier_state | new_state);
}

KeyboardLeds VirtualKeyboard::get_led_state() { return led_state; }

void VirtualKeyboard::set_led_state(KeyboardLeds new_state) {
  led_state = new_state;
}

KeyboardAction VirtualKeyboard::deq() {
  KeyboardAction kb_action;
  if (key_buffer.length() > 0) {
    kb_action = key_buffer.deq();
  } else {
    kb_action.type = KbActionNone;
  }
  return kb_action;
}

void VirtualKeyboard::enq(KeyboardAction kb_action) { key_buffer.enq(kb_action); }
