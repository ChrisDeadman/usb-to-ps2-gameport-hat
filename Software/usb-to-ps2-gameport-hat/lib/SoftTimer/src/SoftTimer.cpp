#include "SoftTimer.h"

SoftTimer::SoftTimer() { reset(); }

unsigned long SoftTimer::getCurrentMillis() { return tCurrentMicros / 1000; }

unsigned long SoftTimer::getCurrentMicros() { return tCurrentMicros; }

unsigned long SoftTimer::getElapsedMillis() { return tElapsedMicros / 1000; }

unsigned long SoftTimer::getElapsedMicros() { return tElapsedMicros; }

void SoftTimer::reset() {
  tStartMicros = tCurrentMicros = micros();
  tElapsedMicros = 0;
}

void SoftTimer::tick() {
  tCurrentMicros = micros();
  tElapsedMicros = tCurrentMicros - tStartMicros;
}
