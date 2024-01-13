#include "HIDMouseCombiner.h"

HIDMouseCombiner::HIDMouseCombiner(HIDMouseController *const controller_1,
                                   HIDMouseController *const controller_2) {
  controllers[0] = controller_1;
  controllers[1] = controller_2;
}

bool HIDMouseCombiner::is_connected() {
  bool connected = false;
  for (uint8_t i = 0; i < num_controllers; i++) {
    connected |= controllers[i]->is_connected();
  }
  return connected;
}

HIDMouseState HIDMouseCombiner::pop_state() {
  bool changed = false;
  state.d_x = 0;
  state.d_y = 0;
  state.d_wheel = 0;
  state.button1 = false;
  state.button2 = false;
  state.button3 = false;
  state.button4 = false;
  state.button5 = false;
  for (uint8_t i = 0; i < num_controllers; i++) {
    uint8_t prev_version_counter = version_counters[i];
    HIDMouseState c_state = controllers[i]->pop_state();
    version_counters[i] = c_state.version_counter;
    if (version_counters[i] != prev_version_counter) {
      state.d_x += c_state.d_x;
      state.d_y += c_state.d_y;
      state.d_wheel += c_state.d_wheel;
      state.button1 |= c_state.button1;
      state.button2 |= c_state.button2;
      state.button3 |= c_state.button3;
      state.button4 |= c_state.button4;
      state.button5 |= c_state.button5;
      changed = true;
    }
  }
  if (changed) {
    state.version_counter += 1;
  }
  return state;
}
