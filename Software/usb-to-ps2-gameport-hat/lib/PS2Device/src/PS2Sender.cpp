#include "PS2Sender.h"

extern "C" {
void __ps2DataSentEmptyCallback(uint8_t dataByte) {}
}
/**
 * implement in your code if you want to capture packages.
 */
void ps2DataSent(uint8_t dataByte)
    __attribute__((weak, alias("__ps2DataSentEmptyCallback")));

bool PS2Sender::isSending() { return sending; }

void PS2Sender::beginSend(uint8_t dataByte) {
  this->dataByte = dataByte;
  bitIdx = 0;
  if (!sending) {
    sending = true;
    port->enableClock();
  }
}

void PS2Sender::endSend() {
  if (!sending) {
    return;
  }
  sending = false;
  port->disableClock();
  if (bitIdx >= 11) {
    ps2DataSent(dataByte);
  }
}

void PS2Sender::resend() {
  if (!sending) {
    return;
  }
  // all bits sent, stop sending
  if (bitIdx >= 11) {
    endSend();
  }
  // resend on next clock
  else {
    bitIdx = 0;
  }
}

void PS2Sender::onClock() {
  if (!sending) {
    return;
  }

  // all bits sent, stop sending
  if (bitIdx >= 11) {
    endSend();
    return;
  }

  uint8_t bit;
  switch (bitIdx) {
    case 0:  // start bit
      bit = 0;
      parity = 1;  // ODD parity
      break;
    default:  // data bits
      bit = (dataByte >> (bitIdx - 1)) & 1;
      parity ^= bit;
      break;
    case 9:  // parity bit
      bit = parity;
      break;
    case 10:  // stop bit
      bit = 1;
      break;
  }
  bitIdx++;

  port->write(bit);
}
