#ifndef _GAMEPORT_H_
#define _GAMEPORT_H_

#include <SPI.h>

class Gameport {
 private:
  const uint8_t W1_ADDRESS = 0;
  const uint8_t W2_ADDRESS = 1;
  const uint8_t W3_ADDRESS = 2;
  const uint8_t W4_ADDRESS = 3;

  const uint8_t POT_CS_PIN;
  const uint8_t BUTTON1_PIN;
  const uint8_t BUTTON2_PIN;
  const uint8_t BUTTON3_PIN;
  const uint8_t BUTTON4_PIN;

 public:
  Gameport(uint8_t potCsPin, uint8_t button1Pin, uint8_t button2Pin,
           uint8_t button3Pin, uint8_t button4Pin)
      : POT_CS_PIN(potCsPin),
        BUTTON1_PIN(button1Pin),
        BUTTON2_PIN(button2Pin),
        BUTTON3_PIN(button3Pin),
        BUTTON4_PIN(button4Pin) {}

  /**
   * Initializes the gameport interface.
   */
  void init();

  /**
   * Sets all button pins to the given pressed states.
   */
  void setButtonStates(bool button1, bool button2, bool button3, bool button4);

  /**
   * Writes all 4 wiper states to the digital potentiometer.
   *
   * Note: there is no wait in between the CS transitions since the pot is
   * specified at 10ns min.pulse-width (48Mhz is ~20ns).
   */
  void setAxes(uint8_t axis1, uint8_t axis2, uint8_t axis3, uint8_t axis4);
};

#endif  // _GAMEPORT_H_
