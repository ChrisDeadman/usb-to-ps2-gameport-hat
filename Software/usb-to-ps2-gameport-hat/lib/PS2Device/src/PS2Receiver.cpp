#include "PS2Receiver.h"

extern "C" {
void __ps2DataReceivedEmptyCallback(uint8_t dataByte, bool dataValid) {}
}
/**
 * implement in your code if you want to capture packages.
 */
void ps2DataReceived(uint8_t dataByte, bool dataValid)
    __attribute__((weak, alias("__ps2DataReceivedEmptyCallback")));

PS2Receiver::PS2Receiver(PS2Port* port) : port(port) {}

bool PS2Receiver::isReceiving() { return receiving; }

bool PS2Receiver::hasData() { return dataPresent; }

bool PS2Receiver::isDataValid() { return dataValid; }

uint8_t PS2Receiver::popData() {
  dataPresent = false;
  return dataByte;
}

void PS2Receiver::beginReceive() {
  dataByte = 0;
  bitIdx = 0;
  dataValid = true;
  dataPresent = false;
  if (!receiving) {
    receiving = true;
    port->enableClock();
  }
}

void PS2Receiver::endReceive() {
  if (!receiving) {
    return;
  }
  receiving = false;
  port->disableClock();
  if (bitIdx >= 11) {
    ps2DataReceived(dataByte, dataValid);
  }
}

void PS2Receiver::onClock() {
  if (!receiving) {
    return;
  }

  // ACK sent, stop receiving
  if (bitIdx >= 11) {
    endReceive();
    return;
  }

  uint8_t bit = port->read();

  switch (bitIdx) {
    case 0:  // start bit
      if (bit != 0) {
        dataValid = false;
      }
      parity = 1;  // ODD parity
      break;
    default:  // data bits
      dataByte |= (bit << (bitIdx - 1));
      parity ^= bit;
      break;
    case 9:  // parity bit
      if (bit != parity) {
        dataValid = false;
      }
      break;
    case 10:  // stop bit
      if (bit != 1) {
        dataValid = false;
      }
      dataPresent = true;
      port->write(dataValid ? 0 : 1);  // ACK / NAK
      break;
  }
  bitIdx++;
}
