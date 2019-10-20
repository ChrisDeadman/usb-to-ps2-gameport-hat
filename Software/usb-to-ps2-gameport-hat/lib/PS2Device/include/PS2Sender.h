#ifndef _PS2_SENDER_H_
#define _PS2_SENDER_H_

#include "PS2Port.h"

class PS2Sender {
 private:
  PS2Port* const port;
  volatile bool sending = false;
  volatile uint8_t dataByte = 0;
  volatile uint8_t bitIdx = 0;
  volatile uint8_t parity = 0;

 public:
  PS2Sender(PS2Port* port) : port(port) {}

  /**
   * Returns whether data is currently being sent.
   */
  bool isSending();

  /**
   * Prepares to transmit data and starts the clock on the port.
   */
  void beginSend(uint8_t dataByte);

  /**
   * Stops the clock on the port and transmission of data.
   */
  void endSend();

  /**
   * Triggers resending of data upon next clock cycle.
   *
   * This has no effect if no data is currently being sent.
   */
  void resend();

  /**
   * Writes one bit of data to the port.
   *
   * Does nothing if not in sending state (beginSend).
   * This should be called on every clock cycle during data transmission.
   */
  void onClock();
};

#endif  //_PS2_SENDER_H_
