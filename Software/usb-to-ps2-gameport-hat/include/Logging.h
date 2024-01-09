#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "JoystickManager.h"
#include "PS2Mouse.h"
#include "SetupMode.h"

class Logging {
 private:
  PS2Mouse* const ps2_mouse;
  JoystickManager* const joystick_manager;
  SetupMode* const setup_mode;

 public:
  Logging(PS2Mouse* const ps2_mouse, JoystickManager* const joystick_manager, SetupMode* const setup_mode);

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
  void logStatus();
};

#endif  //_LOGGING_H_
