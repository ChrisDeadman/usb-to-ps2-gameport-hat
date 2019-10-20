#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "Deadzone.h"
#include "JoystickManager.h"
#include "PS2Mouse.h"

class Logging {
 private:
  PS2Mouse* const ps2Mouse;
  JoystickManager* const joystickManager;
  Deadzone* const deadzone;

 public:
  Logging(PS2Mouse* const ps2Mouse, JoystickManager* const joystickManager,
          Deadzone* const deadzone);

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
