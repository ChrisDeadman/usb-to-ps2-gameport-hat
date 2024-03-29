#include "PS2Device.h"

extern "C" {
void __ps2_dummy_received_callback(uint8_t pin, uint8_t data_byte, bool valid) {}
}

void ps2_data_received(uint8_t pin, uint8_t data_byte, bool valid)
    __attribute__((weak, alias("__ps2_dummy_received_callback")));

extern "C" {
void __ps2_data_sent_dummy_callback(uint8_t pin, uint8_t data_byte) {}
}

void ps2_data_sent(uint8_t pin, uint8_t data_byte)
    __attribute__((weak, alias("__ps2_data_sent_dummy_callback")));

PS2Device::PS2Device(PS2Port* const port)
    : sender(port), receiver(port), port(port) {}

void PS2Device::init() {
  send_buffer_idx = 0;
  send_buffer_len = 0;
  port->set_observer(this);
}

bool PS2Device::is_busy() {
  return receiver.is_busy() || sender.is_busy() ||
         (send_buffer_idx < send_buffer_len);
}

unsigned long PS2Device::get_time_last_inhibit() { return time_last_inhibit; }

unsigned long PS2Device::get_time_last_host_rts() { return time_last_host_rts; }

void PS2Device::on_clock() {
  // transmission is inhibited
  if (port->clock_inhibited) {
    time_last_inhibit = millis();
    receiver.end_receive();
    if (sender.is_busy()) {
      uint8_t bits_sent = sender.bits_sent();
      sender.end_send();
      if (bits_sent < 11) {
        // TODO:
        // somehow the host always expects only the last byte to be retransmitted
        if (true || bits_sent == 0) {
          // inhibited before we could send -> resend after inhibit
          send_buffer_idx -= 1;
        } else {
          // If a transmission is inhibited after the 1st and before the 11th clock
          // pulse, the device must abort the current transmission and prepare to
          // retransmit the current "chunk" of data.
          resend();
        }
      }
    }
  } else {
    receiver.on_clock();
    sender.on_clock();
  }
}

void PS2Device::on_host_rts() {
  time_last_host_rts = millis();
  sender.end_send();
  receiver.begin_receive();
}

void PS2Device::resend() { send_buffer_idx = 0; }

void PS2Device::send_toHost(const uint8_t* data, uint8_t length) {
  memcpy(send_buffer, data, length);
  send_buffer_idx = 0;
  send_buffer_len = length;
}
