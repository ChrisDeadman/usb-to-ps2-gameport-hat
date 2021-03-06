#ifndef _GAMEPORT_H_
#define _GAMEPORT_H_

#include <SPI.h>

/**
 * Modify this to change digital pot calibration.
 *
 * This correction provides a near-linear curve for my setup.
 */
#define AXIS_TO_POT_VALUE(axis) (uint8_t)(0xFF - log10(1.0f + ((axis) / 32.0f)) * 78.0f)

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
  void setButtons(bool button1, bool button2, bool button3, bool button4);

  /**
   * Writes all 4 wiper values to the digital potentiometer.
   *
   * Note: there is no wait in between the CS transitions since the pot is
   * specified at 10ns min.pulse-width (48Mhz is ~20ns).
   */
  void setAxes(uint8_t axis1, uint8_t axis2, uint8_t axis3, uint8_t axis4);
};

#endif  // _GAMEPORT_H_
