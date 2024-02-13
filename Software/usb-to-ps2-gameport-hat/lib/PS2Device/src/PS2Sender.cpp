#include "PS2Sender.h"

extern "C" {
void __ps2_data_sent_dummy_callback(uint8_t pin, uint8_t data) {}
}
/**
 * implement in your code if you want to capture packages.
 */
void ps2_data_sent(uint8_t pin, uint8_t data)
    __attribute__((weak, alias("__ps2_data_sent_dummy_callback")));

PS2Sender::PS2Sender(PS2Port* const port) : port(port) {
  busy = false;
  bit_idx = 0;
  data_byte = 0;
  parity = 0;
}

volatile bool PS2Sender::is_busy() { return busy; }

volatile bool PS2Sender::is_sending() { return busy && (bit_idx > 1); }

volatile void PS2Sender::begin_send(uint8_t data_byte) {
  if (busy) {
    end_send();
  }

  busy = true;
  this->data_byte = data_byte;
  bit_idx = 0;
  port->enable_clock();
}

volatile void PS2Sender::end_send() {
  if (!busy) {
    return;
  }

  busy = false;
  bit_idx = 0;
  port->disable_clock();
}

volatile void PS2Sender::on_clock() {
  if (!busy) {
    return;
  }

  uint8_t bit;
  switch (bit_idx) {
    case 0:  // start bit
      bit = 0;
      parity = 1;  // ODD parity
      break;
    default:  // data bits
      bit = (data_byte >> (bit_idx - 1)) & 1;
      parity ^= bit;
      break;
    case 9:  // parity bit
      bit = parity;
      break;
    case 10:  // stop bit
      bit = 1;
      break;
    case 11:  // transmission complete
      end_send();
      ps2_data_sent(port->data_pin, data_byte);
      return;
  }
  bit_idx++;

  port->write(bit);
}
