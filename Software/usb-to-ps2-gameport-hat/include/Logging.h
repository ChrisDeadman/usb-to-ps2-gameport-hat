#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "HIDKeyboardController.h"
#include "HIDMouseController.h"
#include "HIDMouseKeyboardController.h"
#include "JoystickState.h"
#include "PS2Keyboard.h"
#include "PS2Mouse.h"
#include "SetupMode.h"

class Logging {
 private:
  HIDMouseKeyboardController* const usb_mouse_keyboard;
  HIDKeyboardController* const usb_keyboard;
  HIDMouseController* const usb_mouse;
  MouseState* const mouse_state;
  JoystickState* const joy_state;
  uint8_t* const num_joys_connected;
  PS2Keyboard* const ps2_keyboard;
  PS2Mouse* const ps2_mouse;
  SetupMode* const setup_mode;

 public:
  Logging(HIDMouseKeyboardController* const usb_mouse_keyboard,
          HIDKeyboardController* const usb_keyboard,
          HIDMouseController* const usb_mouse, MouseState* const mouse_state,
          JoystickState* const joy_state, uint8_t* const num_joys_connected,
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
