#ifndef _PS2_SENDER_H_
#define _PS2_SENDER_H_

#include "PS2Port.h"

class PS2Sender {
 private:
  PS2Port* const port;

  volatile bool busy;
  volatile uint8_t bit_idx;
  volatile uint8_t data_byte;
  volatile uint8_t parity;

 public:
  PS2Sender(PS2Port* const port);

  /**
   * Returns whether the sender is busy.
   */
  bool is_busy();

  /**
   * Returns whether data is currently being sent.
   */
  bool is_sending();

  /**
   * Prepares to transmit data and starts the clock on the port.
   */
  void begin_send(uint8_t dataByte);

  /**
   * Stops the clock on the port and transmission of data.
   */
  void end_send();

  /**
   * Writes one bit of data to the port.
   *
   * Does nothing if not in sending state (beginSend).
   * This should be called on every clock cycle during data transmission.
   */
  void on_clock();
};

#endif  //_PS2_SENDER_H_
