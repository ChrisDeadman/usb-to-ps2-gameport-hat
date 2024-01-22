#include "PS2Device.h"

PS2Device::PS2Device(PS2Port* const port)
    : sender(port), receiver(port), port(port) {}

void PS2Device::init() {
  digitalWrite(port->clock_pin, HIGH);
  digitalWrite(port->data_pin, HIGH);

  pinMode(port->clock_pin, OUTPUT);
  pinMode(port->data_pin, OUTPUT);

  port->set_observer(this);
}

bool PS2Device::is_busy() {
  return !port->clock_inhibited && (receiver.is_receiving() || sender.is_sending());
}

unsigned long PS2Device::get_time_last_inhibit() { return time_last_inhibit; }

unsigned long PS2Device::get_time_last_host_rts() { return time_last_host_rts; }

void PS2Device::on_clock() {
  receiver.on_clock();
  sender.on_clock();
}

void PS2Device::on_inhibit() {
  receiver.end_receive();
  sender.resend();
  time_last_inhibit = millis();
}

void PS2Device::on_host_rts() {
  sender.end_send();
  receiver.end_receive();
  receiver.begin_receive();
  time_last_host_rts = millis();
}
