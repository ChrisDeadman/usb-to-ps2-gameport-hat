#ifndef _SETUP_MODE_H_
#define _SETUP_MODE_H_

#include "HIDKeyboardCombiner.h"
#include "JoystickState.h"
#include "SoftPWM.h"
#include "SoftTimer.h"

enum SetupKeys : uint8_t {
  None = 0x0,
  Setup = 0x01,
  Select = 0x02,
  Left = 0x04,
  Right = 0x08,
};

class SetupMode {
 private:
  HIDKeyboardCombiner *const keyboard;
  JoystickState *const joystick_state;
  SetupKeys key_state;

  SoftTimer setup_mode_timer;
  SoftTimer led_update_timer;
  SoftTimer blink_timer;

  const uint8_t item_count = 1;
  int8_t item_idx;

 public:
  SetupMode(HIDKeyboardCombiner *const keyboard,
            JoystickState *const joystick_state);

  bool in_setup_mode;
  bool in_edit_mode;

  bool swap_joy_axis_3_and_4;
  int8_t dummy_value;

  void task();

 private:
  SetupKeys get_key_state();
  void set_led_state(bool led1, bool led2);
  uint8_t get_item_value();
  void set_item_value(int8_t delta);
};

#endif  // _SETUP_MODE_H_
