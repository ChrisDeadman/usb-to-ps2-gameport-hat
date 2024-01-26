#ifndef _GAMEPORT_H_
#define _GAMEPORT_H_

#include <SPI.h>

class Gameport {
 private:
  static const uint8_t W1_ADDRESS = 0;
  static const uint8_t W2_ADDRESS = 1;
  static const uint8_t W3_ADDRESS = 2;
  static const uint8_t W4_ADDRESS = 3;

  const uint8_t POT_CS_PIN;
  const uint8_t BUTTON1_PIN;
  const uint8_t BUTTON2_PIN;
  const uint8_t BUTTON3_PIN;
  const uint8_t BUTTON4_PIN;

 public:
  Gameport(uint8_t potCsPin, uint8_t button1Pin, uint8_t button2Pin,
           uint8_t button3Pin, uint8_t button4Pin);

  /**
   * Initializes the gameport interface.
   */
  void init();

  /**
   * Sets all button pins to the given pressed states.
   */
  void setButtons(uint8_t button1, uint8_t button2, uint8_t button3,
                  uint8_t button4);

  /**
   * Writes all 4 wiper values to the digital potentiometer.
   *
   * Note: there is no wait in between the CS transitions since the pot is
   * specified at 10ns min.pulse-width (48Mhz is ~20ns).
   */
  void setAxes(uint8_t axis1, uint8_t axis2, uint8_t axis3, uint8_t axis4);
};

#endif  // _GAMEPORT_H_
