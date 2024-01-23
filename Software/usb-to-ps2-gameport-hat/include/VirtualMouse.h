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

 private:
  void reset_state();
};

#endif  //_VIRTUAL_MOUSE_H_
