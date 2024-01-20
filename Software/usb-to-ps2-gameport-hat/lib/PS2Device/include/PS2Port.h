#ifndef _PS2_PORT_H_
#define _PS2_PORT_H_

#include <Arduino.h>

#include "PS2PortObserver.h"

class PS2Port {
 private:
  PS2PortObserver *observer;

 public:
  PS2Port(uint8_t clock_pin, uint8_t data_pin);

  const uint8_t clock_pin;
  const uint8_t data_pin;

  /**
   *  ISR vars
   */
  volatile int8_t sub_clock = 0;
  volatile bool clock_enabled = false;
  volatile bool clock_inhibited = false;

  /**
   * Enable clock generation.
   */
  void enable_clock();

  /**
   * Disable clock generation.
   */
  void disable_clock();

  /**
   * Read one bit from the data bus.
   */
  bool read();

  /**
   * Write one bit to the data bus.
   */
  void write(bool bit);

  /**
   * Called by PS/2 devices to set callbacks.
   */
  void set_observer(PS2PortObserver *const observer);

  /**
   * Called upon each clock cycle. This is the time to send or receive data.
   */
  void on_clock();

  /**
   * Called when the host inhibits communication.
   */
  void on_inhibit();

  /**
   * Called when the host requests to send.
   */
  void on_host_rts();

  /**
   * Initializes the PS/2 port interface.
   */
  static void init();

  /**
   * Enable clock interrup request.
   */
  static void enable_clock_irq();

  /**
   * Disable clock interrup request.
   */
  static void disable_clock_irq();
};

#endif  //_PS2_PORT_H_
