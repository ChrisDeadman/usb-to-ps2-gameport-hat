#ifndef _SOFT_PWM_H_
#define _SOFT_PWM_H_

#include <Arduino.h>
#include "SoftTimer.h"

class SoftPWM {
 private:
  static const unsigned long T_PERIOD = 12288;
  static const unsigned long T_FACTOR = T_PERIOD / 128;

  SoftTimer timer;
  unsigned long tDelta;

 public:
  /**
   * Returns a high/low state according to the given value.
   * The higher the value, the more often it will return HIGH (true).
   *
   * This should be called periodically in the loop() function.
   */
  bool pwm(uint8_t value);
};

#endif  //_SOFT_PWM_H_
