#ifndef _SOFT_TIMER_H_
#define _SOFT_TIMER_H_

#include <Arduino.h>

class SoftTimer {
 private:
  unsigned long tStartMicros;
  unsigned long tElapsedMicros;

 public:
  unsigned long getElapsedMillis();
  unsigned long getElapsedMicros();

  SoftTimer();

  void reset();
  void tick();
};

#endif
