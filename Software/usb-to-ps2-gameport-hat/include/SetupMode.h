#ifndef _SETUP_H_
#define _SETUP_H_

#include "JoystickState.h"
#include "SoftPWM.h"
#include "SoftTimer.h"

enum SetupKeys: uint8_t {
  None = 0x0,
  Select = 0x01,
  Left = 0x02,
  Right = 0x04,
};

class SetupMode {
 private:
  JoystickState *const joystick_state;
  SetupKeys key_state;
  SoftTimer setup_timer;
  SoftTimer input_timer;
  SoftTimer blink_timer;
  SoftPWM soft_pwm;

  const uint8_t item_count = 1;
  uint8_t item_idx;

  bool in_edit_mode;

 public:
  SetupMode(JoystickState *const joystick_state);

  bool in_setup_mode;

  bool swap_joy_axis_3_and_4;

  void task();

 private:
  SetupKeys get_key_state();
  uint8_t get_item_value();
  void set_item_value(uint8_t value);
};

#endif  // _SETUP_H_
