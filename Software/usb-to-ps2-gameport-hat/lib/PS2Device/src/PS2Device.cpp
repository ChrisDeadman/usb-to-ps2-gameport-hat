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

void PS2Device::init() { port->set_observer(this); }

bool PS2Device::is_busy() {
  return !port->clock_inhibited && (receiver.is_busy() || sender.is_busy());
}

unsigned long PS2Device::get_time_last_inhibit() { return time_last_inhibit; }

unsigned long PS2Device::get_time_last_host_rts() { return time_last_host_rts; }

volatile void PS2Device::on_clock() {
  receiver.on_clock();
  sender.on_clock();
}

volatile void PS2Device::on_inhibit() {
  time_last_inhibit = millis();
  receiver.end_receive();
  // If a transmission is inhibited after the 1st and before the 11th clock pulse,
  // the device must abort the current transmission and prepare to
  // retransmit the current "chunk" of data.
  if (sender.is_sending()) {
    sender.end_send();
    resend();
  }
}

volatile void PS2Device::on_host_rts() {
  time_last_host_rts = millis();
  sender.end_send();
  receiver.end_receive();
  receiver.begin_receive();
}
