#ifndef HID_MOUSE_COMBINER_H
#define HID_MOUSE_COMBINER_H

#include <Arduino.h>
#include <hidboot.h>

#include "HIDMouseController.h"

class HIDMouseCombiner {
 private:
  static const uint8_t num_controllers = 2;
  HIDMouseController *controllers[num_controllers];
  uint8_t version_counters[num_controllers];
  HIDMouseState state;

 public:
  HIDMouseCombiner(HIDMouseController *const controller_1,
                   HIDMouseController *const controller_2);

  /**
   * Returns whether a supported device is connected.
   */
  bool is_connected();

  /**
   * Returns the latest mouse state and resets it.
   *
   * Mouse deltas are summed up until popState is called.
   * This should be called periodically.
   */
  HIDMouseState pop_state();
};

#endif  // HID_MOUSE_COMBINER_H
