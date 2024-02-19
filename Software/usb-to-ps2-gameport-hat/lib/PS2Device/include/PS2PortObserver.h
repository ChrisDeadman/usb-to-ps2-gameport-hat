#ifndef _PS2_PORT_OBSERVER_H_
#define _PS2_PORT_OBSERVER_H_

#include <Arduino.h>

/**
 * Interface for PS2Port observers.
 */
class PS2PortObserver {
 public:
  /**
   * Called upon each clock cycle. This is the time to send or receive data.
   */
  virtual void on_clock() = 0;

  /**
   * Called when the host inhibits communication.
   */
  virtual void on_inhibit() = 0;

  /**
   * Called when the host requests to send.
   */
  virtual void on_host_rts() = 0;
};

#endif  //_PS2_PORT_OBSERVER_H_
