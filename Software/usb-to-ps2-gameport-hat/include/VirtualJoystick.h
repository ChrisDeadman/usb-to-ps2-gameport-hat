#ifndef _VIRTUAL_JOYSTICK_H_
#define _VIRTUAL_JOYSTICK_H_

#include "JoystickState.h"

class VirtualJoystick {
 private:
  JoystickState state;

 public:
  VirtualJoystick();

  /**
   * Returns the current state, then resets it.
   *
   * Deltas are summed up until pop_state is called.
   * This should be called periodically.
   */
  JoystickState pop_state();

  /**
   * Updates the current state and marks it as changed.
   *
   * Deltas are summed up until pop_state is called.
   */
  void update_state(JoystickState const* const new_state, bool is_player_1,
                    bool swap_joy_axis_3_and_4);

  /**
   * Combines two axis.
   */
  static uint8_t combine_axis(uint8_t value_a, uint8_t value_b) {
    int16_t combined = ((int16_t)value_a - 0x80) + ((int16_t)value_b - 0x80) + 0x80;
    if (combined < 0) combined = 0;
    if (combined > UINT8_MAX) combined = UINT8_MAX;
    return (uint8_t)combined;
  }

 private:
  void reset_state();
};

#endif  //_VIRTUAL_JOYSTICK_H_
