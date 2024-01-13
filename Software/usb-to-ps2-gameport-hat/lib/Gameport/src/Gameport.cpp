#include "Gameport.h"

static void spiTransfer(uint8_t csPin, uint8_t address, uint8_t data);

Gameport::Gameport(uint8_t potCsPin, uint8_t button1Pin, uint8_t button2Pin,
                   uint8_t button3Pin, uint8_t button4Pin)
    : POT_CS_PIN(potCsPin),
      BUTTON1_PIN(button1Pin),
      BUTTON2_PIN(button2Pin),
      BUTTON3_PIN(button3Pin),
      BUTTON4_PIN(button4Pin) {}

void Gameport::init() {
  digitalWrite(POT_CS_PIN, HIGH);
  digitalWrite(BUTTON1_PIN, HIGH);
  digitalWrite(BUTTON2_PIN, HIGH);
  digitalWrite(BUTTON3_PIN, HIGH);
  digitalWrite(BUTTON4_PIN, HIGH);

  pinMode(POT_CS_PIN, OUTPUT);
  pinMode(BUTTON1_PIN, OUTPUT);
  pinMode(BUTTON2_PIN, OUTPUT);
  pinMode(BUTTON3_PIN, OUTPUT);
  pinMode(BUTTON4_PIN, OUTPUT);

  SPI.begin();

  setButtons(false, false, false, false);
  setAxes(0x80, 0x80, 0x80, 0x80);
}

void Gameport::setButtons(bool button1, bool button2, bool button3,
                          bool button4) {
  digitalWrite(BUTTON1_PIN, button1 ? LOW : HIGH);
  digitalWrite(BUTTON2_PIN, button2 ? LOW : HIGH);
  digitalWrite(BUTTON3_PIN, button3 ? LOW : HIGH);
  digitalWrite(BUTTON4_PIN, button4 ? LOW : HIGH);
}

void Gameport::setAxes(uint8_t axis1, uint8_t axis2, uint8_t axis3,
                       uint8_t axis4) {
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  spiTransfer(POT_CS_PIN, W1_ADDRESS, AXIS_TO_POT_VALUE(axis1));
  spiTransfer(POT_CS_PIN, W2_ADDRESS, AXIS_TO_POT_VALUE(axis2));
  spiTransfer(POT_CS_PIN, W3_ADDRESS, AXIS_TO_POT_VALUE(axis3));
  spiTransfer(POT_CS_PIN, W4_ADDRESS, AXIS_TO_POT_VALUE(axis4));
  SPI.endTransaction();
}

static void spiTransfer(uint8_t csPin, uint8_t address, uint8_t data) {
  digitalWrite(csPin, LOW);
  SPI.transfer(address);
  SPI.transfer(data);
  digitalWrite(csPin, HIGH);
}
