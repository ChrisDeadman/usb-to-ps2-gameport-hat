#ifndef _SOFT_TIMER_H_
#define _SOFT_TIMER_H_

#include <Arduino.h>

class SoftTimer {
 private:
  unsigned long tStartMicros;
  unsigned long tCurrentMicros;
  unsigned long tElapsedMicros;

 public:
  SoftTimer();

  /**
   * Returns the current time in milliseconds.
   *
   * Updated during last call to tick().
   */
  unsigned long getCurrentMillis();

  /**
   * Returns the current time in microseconds.
   *
   * Updated during last call to tick().
   */
  unsigned long getCurrentMicros();

  /**
   * Returns the elapsed time in milliseconds.
   *
   * Updated during last call to tick().
   */
  unsigned long getElapsedMillis();

  /**
   * Returns the elapsed time in microseconds.
   *
   * Updated during last call to tick().
   */
  unsigned long getElapsedMicros();

  /**
   * Resets the timer.
   */
  void reset();

  /**
   * Updates the timer values.
   *
   * This should be called periodically in the loop() function.
   */
  void tick();
};

#endif  //_SOFT_TIMER_H_
