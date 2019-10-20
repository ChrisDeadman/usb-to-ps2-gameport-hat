#include "SoftTimer.h"

unsigned long SoftTimer::getElapsedMillis() { return tElapsedMicros / 1000; }

unsigned long SoftTimer::getElapsedMicros() { return tElapsedMicros; }

SoftTimer::SoftTimer() { reset(); }

void SoftTimer::reset() {
  tStartMicros = micros();
  tElapsedMicros = 0;
}

void SoftTimer::tick() {
  unsigned long tNow = micros();
  if (tNow < tStartMicros) {
    tElapsedMicros = (((unsigned long)-1) - tStartMicros) + tNow;
  } else {
    tElapsedMicros = tNow - tStartMicros;
  }
}
