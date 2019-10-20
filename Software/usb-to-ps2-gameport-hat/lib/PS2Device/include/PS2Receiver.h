#ifndef _PS2_RECEIVER_H_
#define _PS2_RECEIVER_H_

#include "PS2Port.h"

class PS2Receiver {
 private:
  PS2Port* const port;
  volatile bool receiving = false;
  volatile bool dataPresent = false;
  volatile bool dataValid = false;
  volatile uint8_t dataByte = 0;
  volatile uint8_t bitIdx = 0;
  volatile uint8_t parity = 0;

 public:
  PS2Receiver(PS2Port* port);

  /**
   * Returns whether data is currently being received.
   */
  bool isReceiving();

  /**
   * Returns whether new data is available.
   */
  bool hasData();

  /**
   * Returns whether the available data is valid.
   *
   * Correctness of start-/stop- and parity-bits is checked.
   */
  bool isDataValid();

  /**
   * Returns available data and clears the data available state.
   */
  uint8_t popData();

  /**
   * Prepares to receive data and starts the clock on the port.
   */
  void beginReceive();

  /**
   * Stops the clock on the port and reception of data.
   */
  void endReceive();

  /**
   * Reads one bit of data from the port.
   *
   * Does nothing if not in receiving state (beginReceive).
   * This should be called on every clock cycle during data reception.
   */
  void onClock();
};

#endif  //_PS2_RECEIVER_H_
