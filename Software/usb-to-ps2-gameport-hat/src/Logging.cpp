#include "Logging.h"
#include "Config.h"
#include "GlobalStringBuffer.h"

GlobalStringBuffer* logBuffer = GlobalStringBuffer::alloc(2048);

volatile unsigned long tCurrent;
volatile unsigned long tLastReceived;
volatile unsigned long tLastSent;

extern "C" char* sbrk(int incr);
int getFreeMemory() {
  uint8_t stack_top;
  return &stack_top - reinterpret_cast<uint8_t*>(sbrk(0));
}

/**
 * Callback, captures packets from PS2Receiver.
 */
void ps2DataReceived(uint8_t dataByte, bool dataValid) {
  tLastReceived = tCurrent;
  logBuffer->concat("<= ")
      ->concat("%02X", dataByte)
      ->concatln(dataValid ? "" : "!!");
}

/**
 * Callback, captures packets from PS2Sender.
 */
void ps2DataSent(uint8_t dataByte) {
  tLastSent = tCurrent;
  logBuffer->concat("=> ")->concatln("%02X", dataByte);
}

void printAndFlushBuffer() {
  if (!logBuffer->isEmpty()) {
    Serial.println(logBuffer->get());
    logBuffer->clear();
  }
}

Logging::Logging(PS2Mouse* const ps2Mouse,
                 JoystickManager* const joystickManager,
                 Deadzone* const deadzone)
    : ps2Mouse(ps2Mouse),
      joystickManager(joystickManager),
      deadzone(deadzone) {}

void Logging::init() { Serial.begin(SERIAL_SPEED); }

void Logging::task() {
  tCurrent = millis();

  // print log if someone writes to our serial input
  if (Serial.available() && Serial.read()) {
    printAndFlushBuffer();
    logStatus();
    printAndFlushBuffer();
  }
}

void Logging::logStatus() {
  unsigned long tLastInhibit = ps2Mouse->getTimeLastInhibit();
  unsigned long tLastHostRts = ps2Mouse->getTimeLastHostRts();

  uint8_t numConnectedDevices = joystickManager->getNumConnectedDevices();
  JoystickState joy1State = joystickManager->getControllerState(0);
  JoystickState joy2State = joystickManager->getControllerState(1);

  logBuffer->concatln("-------------------------------");
  logBuffer->concatln("USB => PS/2 & Gameport HAT V1.0");
  logBuffer->concatln("-------------------------------");
  logBuffer->concatln("General status");
  logBuffer->concatln("--------------");
  logBuffer->concat("free memory: ")->concatln("%d", getFreeMemory());
  logBuffer->concat("time: ")->concatln("%lu", tCurrent);
  logBuffer->concat("ext led1: ")->concatln("%u", digitalRead(EXT_LED1_PIN));
  logBuffer->concat("ext led2: ")->concatln("%u", digitalRead(EXT_LED2_PIN));
  logBuffer->concatln("-----------------");
  logBuffer->concatln("PS/2 mouse status");
  logBuffer->concatln("-----------------");
  logBuffer->concat("device id: ")->concatln("%d", ps2Mouse->getDeviceId());
  logBuffer->concat("clock: ")->concatln("%u", digitalRead(PS2_CLOCK_PIN));
  logBuffer->concat("data: ")->concatln("%u", digitalRead(PS2_DATA_PIN));
  logBuffer->concat("last received time: ")->concatln("%lu", tLastReceived);
  logBuffer->concat("last sent time: ")->concatln("%lu", tLastSent);
  logBuffer->concat("last inhibit time: ")->concatln("%lu", tLastInhibit);
  logBuffer->concat("last host RTS time: ")->concatln("%lu", tLastHostRts);
  logBuffer->concatln("---------------");
  logBuffer->concatln("Joystick status");
  logBuffer->concatln("---------------");
  logBuffer->concat("connected devices: ")->concatln("%u", numConnectedDevices);
  logBuffer->concat("deadzone: ");
  logBuffer->concatln("%u%%", (uint8_t)(deadzone->getValue() * 100));
  logBuffer->concat("Joy1 buttons: ");
  for (uint8_t button = 0; button < 6; button++) {
    logBuffer->concat("%u ", joy1State.buttons[button]);
  }
  logBuffer->concatln("");
  logBuffer->concat("Joy2 buttons: ");
  for (uint8_t button = 0; button < 6; button++) {
    logBuffer->concat("%u ", joy2State.buttons[button]);
  }
  logBuffer->concatln("");
  logBuffer->concat("Joy1 axes: ");
  for (uint8_t axis = 0; axis < 4; axis++) {
    logBuffer->concat("%02X ", joy1State.axes[axis]);
  }
  logBuffer->concatln("");
  logBuffer->concat("Joy2 axes: ");
  for (uint8_t axis = 0; axis < 4; axis++) {
    logBuffer->concat("%02X ", joy2State.axes[axis]);
  }
  logBuffer->concatln("");
  logBuffer->concatln("-------------------------------");
}
