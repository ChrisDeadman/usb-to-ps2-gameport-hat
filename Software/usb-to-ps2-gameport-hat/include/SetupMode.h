#ifndef _SETUP_MODE_H_
#define _SETUP_MODE_H_

#include "JoystickState.h"
#include "SoftPWM.h"
#include "SoftTimer.h"
#include "VirtualKeyboard.h"

enum SetupKeys : uint16_t {
  SetupKeyNone = 0x0,
  SetupKeySetup = 0x01,
  SetupKeySelect = 0x02,
  SetupKeyLeft = 0x04,
  SetupKeyRight = 0x08,
  SetupKeyQuick0 = 0x1000,
  SetupKeyQuick1 = 0x1100,
  SetupKeyQuick2 = 0x1200,
  SetupKeyQuick3 = 0x1300,
  SetupKeyQuick4 = 0x1400,
  SetupKeyQuick5 = 0x1500,
  SetupKeyQuick6 = 0x1600
};

enum EmuMode : uint8_t {
  EmuModeNone = 0,
  EmuModeJoyKeyb = 1,
  EmuModeJoyMouse = 2,
  EmuModeKeybJoy = 3,
  EmuModeKeybMouse = 4,
  EmuModeMouseJoy = 5,
};

class SetupMode {
 private:
  VirtualKeyboard *const keyboard;
  JoystickState *const joystick_state;

  const uint8_t item_count = 2;
  uint8_t item_idx;
  SetupKeys prev_key_state;

  SoftTimer setup_mode_timer;
  SoftTimer blink_timer;

 public:
  SetupMode(VirtualKeyboard *const keyboard, JoystickState *const joystick_state);

  bool in_setup_mode;
  bool in_edit_mode;

  bool swap_joy_axis_3_and_4;
  EmuMode emu_mode;

  void task();

 private:
  void reset_state();
  SetupKeys get_key_state();
  void set_led_state(bool led1, bool led2);
  uint8_t get_item_value();
  void set_item_value(int8_t delta);
  bool set_item_value_quick(SetupKeys keys);
};

#endif  // _SETUP_MODE_H_
