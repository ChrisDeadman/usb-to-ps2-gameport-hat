#include "PS2Device.h"

PS2Device::PS2Device(PS2Port* port) : sender(port), receiver(port) {
  port->registerObserver(this);
}

bool PS2Device::isBusy() {
  return receiver.isReceiving() || sender.isSending();
}

unsigned long PS2Device::getTimeLastInhibit() { return timeLastInhibit; }

unsigned long PS2Device::getTimeLastHostRts() { return timeLastHostRts; }

void PS2Device::onClock() {
  receiver.onClock();
  sender.onClock();
}

void PS2Device::onInhibit() {
  receiver.endReceive();
  sender.resend();
  if (sender.isSending()) timeLastInhibit = millis();
}

void PS2Device::onHostRts() {
  sender.endSend();
  receiver.endReceive();
  receiver.beginReceive();
  timeLastHostRts = millis();
}
