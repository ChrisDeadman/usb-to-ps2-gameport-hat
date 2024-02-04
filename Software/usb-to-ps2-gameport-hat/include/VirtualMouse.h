#ifndef _VIRTUAL_MOUSE_H_
#define _VIRTUAL_MOUSE_H_

#include "MouseState.h"

class VirtualMouse {
 private:
  MouseState state;

 public:
  VirtualMouse();

  /**
   * Returns the current state, then resets it.
   *
   * Deltas are summed up until pop_state is called.
   * This should be called periodically.
   */
  MouseState pop_state();

  /**
   * Updates the current state and marks it as changed.
   *
   * Deltas are summed up until pop_state is called.
   */
  void update_state(MouseState const* const state);

  /**
   * Combines two axis.
   */
  static int8_t combine_axis(int8_t value_a, int8_t value_b) {
    int16_t combined = value_a + value_b;
    if (combined < INT8_MIN) {
      combined = INT8_MIN;
    } else if (combined > INT8_MAX) {
      combined = INT8_MAX;
    }
    return (int8_t)combined;
  }

 private:
  void reset_state();
};

#endif  //_VIRTUAL_MOUSE_H_
