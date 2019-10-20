#ifndef _SOFT_PWM_H_
#define _SOFT_PWM_H_

#include <Arduino.h>
#include "SoftTimer.h"

class SoftPWM {
 private:
  const unsigned long tPeriod = 19890;
  const unsigned long tFactor = tPeriod / 255;

  SoftTimer timer;
  unsigned long tDelta;

 public:
  bool pwm(uint8_t value);
};

#endif
