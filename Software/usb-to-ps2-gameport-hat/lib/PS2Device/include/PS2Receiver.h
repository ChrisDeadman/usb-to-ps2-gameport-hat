#ifndef _PS2_RECEIVER_H_
#define _PS2_RECEIVER_H_

#include "PS2Port.h"

class PS2Receiver {
 private:
  PS2Port* const port;
  volatile bool receiving;
  volatile bool data_present;
  volatile bool data_valid;
  volatile uint8_t data_byte;
  volatile uint8_t bit_idx;
  volatile uint8_t parity;

 public:
  PS2Receiver(PS2Port* const port);

  /**
   * Returns whether data is currently being received.
   */
  bool is_receiving();

  /**
   * Returns whether new data is available.
   */
  bool has_data();

  /**
   * Returns whether the available data is valid.
   *
   * Correctness of start-/stop- and parity-bits is checked.
   */
  bool is_data_valid();

  /**
   * Returns available data and clears the data available state.
   */
  uint8_t pop_data();

  /**
   * Prepares to receive data and starts the clock on the port.
   */
  void begin_receive();

  /**
   * Stops the clock on the port and reception of data.
   */
  bool end_receive();

  /**
   * Reads one bit of data from the port.
   *
   * Does nothing if not in receiving state (beginReceive).
   * This should be called on every clock cycle during data reception.
   */
  void on_clock();
};

#endif  //_PS2_RECEIVER_H_
