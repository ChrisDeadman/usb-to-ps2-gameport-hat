#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

class Watchdog {
 public:
  /**
   * Perform the necessary platform setup.
   *
   * The hard-coded period is 8s.
   */
  void init();

  /**
   * Reset the watchdog timer.
   *
   * This should be called periodically in the loop() function.
   */
  void reset();
};

#endif  //_WATCHDOG_H_
