#include "SoftPWM.h"

bool SoftPWM::pwm(uint8_t value) {
  timer.tick();
  tDelta = timer.getElapsedMicros();

  if (tDelta >= T_PERIOD) {
    timer.reset();
    tDelta = 0;
  }

  return (tDelta < (value * T_FACTOR));
}
