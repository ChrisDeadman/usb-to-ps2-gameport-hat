#include "Gameport.h"

void spiTransfer(uint8_t csPin, uint8_t address, uint8_t data) {
  digitalWrite(csPin, LOW);
  SPI.transfer(address);
  SPI.transfer(data);
  digitalWrite(csPin, HIGH);
}

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
}

void Gameport::setButtonStates(bool button1, bool button2, bool button3,
                               bool button4) {
  digitalWrite(BUTTON1_PIN, button1);
  digitalWrite(BUTTON2_PIN, button2);
  digitalWrite(BUTTON3_PIN, button3);
  digitalWrite(BUTTON4_PIN, button4);
}

void Gameport::setAxes(uint8_t axis1, uint8_t axis2, uint8_t axis3,
                       uint8_t axis4) {
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  spiTransfer(POT_CS_PIN, W1_ADDRESS, axis1);
  spiTransfer(POT_CS_PIN, W2_ADDRESS, axis2);
  spiTransfer(POT_CS_PIN, W3_ADDRESS, axis3);
  spiTransfer(POT_CS_PIN, W4_ADDRESS, axis4);
  SPI.endTransaction();
}
