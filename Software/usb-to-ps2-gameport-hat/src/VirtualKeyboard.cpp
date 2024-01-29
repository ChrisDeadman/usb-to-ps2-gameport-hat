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

KeyboardAction VirtualKeyboard::deq() {
  KeyboardAction kb_action;
  if (action_buffer.length() > 0) {
    kb_action = action_buffer.deq();
  } else {
    kb_action.type = KbActionNone;
  }
  return kb_action;
}

void VirtualKeyboard::update_modifier_state(KeyboardModifierState new_state) {
  modifier_state = (KeyboardModifierState)(modifier_state | new_state);
}

void VirtualKeyboard::update_led_state(KeyboardLeds new_state) {
  led_state = (KeyboardLeds)(led_state | new_state);
}

void VirtualKeyboard::enq(KeyboardAction kb_action) { action_buffer.enq(kb_action); }
