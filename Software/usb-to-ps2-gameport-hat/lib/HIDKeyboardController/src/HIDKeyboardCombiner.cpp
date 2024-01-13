#include "HIDKeyboardCombiner.h"

HIDKeyboardCombiner::HIDKeyboardCombiner(
    HIDKeyboardController *const controller_1,
    HIDKeyboardController *const controller_2) {
  controllers[0] = controller_1;
  controllers[1] = controller_2;
}

bool HIDKeyboardCombiner::is_connected() {
  bool connected = false;
  for (uint8_t i = 0; i < num_controllers; i++) {
    connected |= controllers[i]->is_connected();
  }
  return connected;
}

ModifierState HIDKeyboardCombiner::get_modifier_state() {
  ModifierState state = ModifierState::ModNone;
  for (uint8_t i = 0; i < num_controllers; i++) {
    state = (ModifierState)(state | controllers[i]->get_modifier_state());
  }
  return state;
}

KeyboardCodes HIDKeyboardCombiner::deq_make() {
  for (uint8_t i = 0; i < num_controllers; i++) {
    KeyboardCodes key = controllers[i]->deq_make();
    if (key != KeyboardCodes::NoKey) {
      return key;
    }
  }
  return KeyboardCodes::NoKey;
}

KeyboardCodes HIDKeyboardCombiner::deq_brk() {
  for (uint8_t i = 0; i < num_controllers; i++) {
    KeyboardCodes key = controllers[i]->deq_brk();
    if (key != KeyboardCodes::NoKey) {
      return key;
    }
  }
  return KeyboardCodes::NoKey;
}

void HIDKeyboardCombiner::set_led_state(KeyboardLeds state) {
  for (uint8_t i = 0; i < num_controllers; i++) {
    controllers[i]->set_led_state(state);
  }
}
