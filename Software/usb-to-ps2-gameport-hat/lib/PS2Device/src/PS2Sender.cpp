#include "PS2Sender.h"

extern "C" {
void __ps2_data_sent_dummy_callback(uint8_t data_byte) {}
}
/**
 * implement in your code if you want to capture packages.
 */
void ps2_data_sent(uint8_t data_byte) __attribute__((weak, alias("__ps2_data_sent_dummy_callback")));

PS2Sender::PS2Sender(PS2Port* const port) : port(port) {}

bool PS2Sender::is_sending() { return sending; }

void PS2Sender::begin_send(uint8_t data_byte) {
  this->data_byte = data_byte;
  bit_idx = 0;
  if (!sending) {
    sending = true;
    port->enable_clock();
  }
}

void PS2Sender::end_send() {
  if (!sending) {
    return;
  }
  sending = false;
  port->disable_clock();
  if (bit_idx >= 11) {
    ps2_data_sent(data_byte);
  }
}

void PS2Sender::resend() {
  if (!sending) {
    return;
  }
  // all bits sent, stop sending
  if (bit_idx >= 11) {
    end_send();
  }
  // resend on next clock
  else {
    bit_idx = 0;
  }
}

void PS2Sender::on_clock() {
  if (!sending) {
    return;
  }

  // all bits sent, stop sending
  if (bit_idx >= 11) {
    end_send();
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
  }
  bit_idx++;

  port->write(bit);
}
