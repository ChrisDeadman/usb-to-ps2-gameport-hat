#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "HIDKeyboardCombiner.h"
#include "HIDMouseCombiner.h"
#include "JoystickManager.h"
#include "PS2Keyboard.h"
#include "PS2Mouse.h"
#include "SetupMode.h"

class Logging {
 private:
  HIDKeyboardCombiner* const usb_keyboard;
  HIDMouseCombiner* const usb_mouse;
  JoystickManager* const joystick_manager;
  PS2Keyboard* const ps2_keyboard;
  PS2Mouse* const ps2_mouse;
  SetupMode* const setup_mode;

 public:
  Logging(HIDKeyboardCombiner* const usb_keyboard,
          HIDMouseCombiner* const usb_mouse,
          JoystickManager* const joystick_manager,
          PS2Keyboard* const ps2_keyboard, PS2Mouse* const ps2_mouse,
          SetupMode* const setup_mode);

  /**
   * Initializes the serial interface.
   */
  void init();

  /**
   * Logging main task.
   *
   * This should be called periodically in the loop() function.
   */
  void task();

 private:
  void log_status();
};

#endif  //_LOGGING_H_
