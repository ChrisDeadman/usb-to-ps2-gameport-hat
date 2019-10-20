#include "SoftPWM.h"

bool SoftPWM::pwm(uint8_t value) {
  timer.tick();
  tDelta = timer.getElapsedMicros();

  if (tDelta >= tPeriod) {
    timer.reset();
    tDelta = 0;
  }

  return (tDelta < (value * tFactor));
}
