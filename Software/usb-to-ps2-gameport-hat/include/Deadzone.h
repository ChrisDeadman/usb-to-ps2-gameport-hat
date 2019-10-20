#ifndef _DEADZONE_H_
#define _DEADZONE_H_

#include <Arduino.h>

class Deadzone {
 private:
  static constexpr float DEADZONE_STEP_VALUE = 0.1f;
  static const uint8_t DEADZONE_FACTOR_MIN = 0;
  static const uint8_t DEADZONE_FACTOR_MAX = 9;

  uint8_t deadzoneFactor = DEADZONE_FACTOR_MIN;

 public:
  /**
   * Returns whether the deadzone factor is greater than zero.
   */
  bool isEnabled();

  /**
   * Calculates the deadzone value based on the deadzone factor and returns it.
   */
  float getValue();

  /**
   * Increases the deadzone factor up to DEADZONE_FACTOR_MAX.
   */
  void increase();

  /**
   * Decreases the deadzone factor up to DEADZONE_FACTOR_MIN.
   */
  void decrease();

  /**
   * Apply the deadzone to the given values.
   *
   * inputs/outputs are between -1.0 and 1.0.
   */
  void apply(float* const x, float* const y);
};

#endif
