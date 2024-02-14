#include "PS2Receiver.h"

PS2Receiver::PS2Receiver(PS2Port* const port) : port(port) {
  busy = false;
  data_present = false;
  data_valid = false;
  bit_idx = 0;
  data_byte = 0;
  parity = 0;
}

volatile bool PS2Receiver::is_busy() { return busy; }

volatile bool PS2Receiver::has_data() { return data_present; }

volatile bool PS2Receiver::is_data_valid() { return data_valid; }

volatile uint8_t PS2Receiver::pop_data() {
  data_present = false;
  return data_byte;
}

volatile void PS2Receiver::begin_receive() {
  if (busy) {
    end_receive();
  }

  busy = true;
  data_present = false;
  data_valid = true;
  bit_idx = 0;
  data_byte = 0;
  port->enable_clock();
}

volatile void PS2Receiver::end_receive() {
  if (!busy) {
    return;
  }

  busy = false;
  bit_idx = 0;
  port->disable_clock();
}

volatile void PS2Receiver::on_clock() {
  if (!busy) {
    return;
  }

  uint8_t bit = port->read();

  switch (bit_idx) {
    case 0:  // start bit
      if (bit != 0) {
        data_valid = false;
      }
      parity = 1;  // ODD parity
      break;
    default:  // data bits
      data_byte |= (bit << (bit_idx - 1));
      parity ^= bit;
      break;
    case 9:  // parity bit
      if (bit != parity) {
        data_valid = false;
      }
      break;
    case 10:  // stop bit
      if (bit != 1) {
        data_valid = false;
      }
      data_present = true;
      port->write(data_valid ? 0 : 1);  // ACK / NAK
      break;
    case 11:  // transmission complete
      end_receive();
      return;
  }
  bit_idx++;
}
