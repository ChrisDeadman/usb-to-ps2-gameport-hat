#ifndef _PS2_PORT_H_
#define _PS2_PORT_H_

#include <Arduino.h>
#include "PS2PortObserver.h"

class PS2Port {
 public:
  PS2Port(uint8_t clockPin, uint8_t dataPin, uint8_t statusPin);

  /**
   * Initializes the PS/2 port interface.
   */
  void init();

  /**
   * Initializes the PS/2 port interface.
   */
  void registerObserver(PS2PortObserver* const observer);

  /**
   * Enable clock generation.
   */
  void enableClock();

  /**
   * Disable clock generation.
   */
  void disableClock();

  /**
   * Enable clock interrup request.
   */
  void enableClockIrq();

  /**
   * Disable clock interrup request.
   */
  void disableClockIrq();

  /**
   * Write one bit to the data bus.
   */
  void write(bool bit);

  /**
   * Read one bit from the data bus.
   */
  bool read();
};

#endif  //_PS2_PORT_H_
