#ifndef _JOYSTICK_DRIVER_MAPPER_H_
#define _JOYSTICK_DRIVER_MAPPER_H_

#include "JoystickState.h"

class JoystickDriverMapper {
 public:
  /**
   * Returns the number of connected devices.
   */
  virtual uint8_t getNumConnectedDevices() = 0;

  /**
   * Returns the state of the controller with the specified index.
   */
  virtual JoystickState getControllerState(uint8_t idx) = 0;
};

#endif  //_JOYSTICK_DRIVER_MAPPER_H_
