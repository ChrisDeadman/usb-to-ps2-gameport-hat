#include "PS2Receiver.h"

extern "C" {
void __ps2_dummy_received_callback(uint8_t pin, uint8_t data, bool valid) {}
}
/**
 * implement in your code if you want to capture packages.
 */
void ps2_data_received(uint8_t pin, uint8_t data, bool valid)
    __attribute__((weak, alias("__ps2_dummy_received_callback")));

PS2Receiver::PS2Receiver(PS2Port* const port) : port(port) {
  receiving = false;
  data_present = false;
  data_valid = false;
  data_byte = 0;
  bit_idx = 0;
  parity = 0;
}

bool PS2Receiver::is_receiving() { return receiving; }

bool PS2Receiver::has_data() { return data_present; }

bool PS2Receiver::is_data_valid() { return data_valid; }

uint8_t PS2Receiver::pop_data() {
  data_present = false;
  return data_byte;
}

void PS2Receiver::begin_receive() {
  data_byte = 0;
  bit_idx = 0;
  data_valid = true;
  data_present = false;
  if (!receiving) {
    receiving = true;
    port->enable_clock();
  }
}

bool PS2Receiver::end_receive() {
  bool finished = bit_idx >= 11;

  if (!receiving) {
    return true;
  }

  port->disable_clock();
  bit_idx = 0;
  receiving = false;

  if (finished) {
    ps2_data_received(port->data_pin, data_byte, data_valid);
  }
  return finished;
}

void PS2Receiver::on_clock() {
  if (!receiving) {
    return;
  }

  // ACK sent, stop receiving
  if (bit_idx >= 11) {
    end_receive();
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
  }
  bit_idx++;
}
