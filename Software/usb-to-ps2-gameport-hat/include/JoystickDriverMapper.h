#ifndef _JOYSTICK_DRIVER_MAPPER_H_
#define _JOYSTICK_DRIVER_MAPPER_H_

#include "JoystickLeds.h"
#include "JoystickState.h"

class IJoystickDriverMapper {
 public:
  /**
   * Returns the number of connected devices.
   */
  virtual uint8_t get_num_connected_devices() = 0;

  /**
   * Returns the current state of the controller at `idx`, then resets it.
   *
   * Deltas are summed up until pop_state is called.
   * This should be called periodically.
   */
  virtual JoystickState pop_state(uint8_t idx) = 0;

  /**
   * [Optional] Sets the state of the joystick leds.
   */
  virtual void set_led_state(JoystickLeds new_state) {}
};

#endif  //_JOYSTICK_DRIVER_MAPPER_H_
