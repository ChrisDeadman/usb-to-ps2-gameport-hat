#include <Arduino.h>
#include "Config.h"

#include "SoftPWM.h"
SoftPWM softPwm;

#include "SoftTimer.h"
SoftTimer timer;

#include "Gameport.h"
Gameport gameport(POT1_CS_PIN, JOY_BUTTON1_PIN, JOY_BUTTON2_PIN,
                  JOY_BUTTON3_PIN, JOY_BUTTON4_PIN);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(EXT_LED1_PIN, OUTPUT);
  pinMode(EXT_LED2_PIN, OUTPUT);

  pinMode(PS2_DATA_PIN, OUTPUT);
  pinMode(PS2_CLOCK_PIN, OUTPUT);

  gameport.init();

  timer.reset();
}

uint8_t builtInLed_value = 0;
uint8_t extLed1_value = 0;
uint8_t extLed2_value = 128;

uint8_t ps2Data_value = 0;
uint8_t ps2Clock_value = 128;

uint8_t joyButton1_value = 0;
uint8_t joyButton2_value = 64;
uint8_t joyButton3_value = 128;
uint8_t joyButton4_value = 192;

uint8_t joyAxis1_value = 0;
uint8_t joyAxis2_value = 64;
uint8_t joyAxis3_value = 128;
uint8_t joyAxis4_value = 192;

void incrementValues() {
  ++builtInLed_value;
  ++extLed1_value;
  ++extLed2_value;

  ++ps2Data_value;
  ++ps2Clock_value;

  ++joyButton1_value;
  ++joyButton2_value;
  ++joyButton3_value;
  ++joyButton4_value;

  ++joyAxis1_value;
  ++joyAxis2_value;
  ++joyAxis3_value;
  ++joyAxis4_value;
}

void loop() {
  digitalWrite(LED_BUILTIN, softPwm.pwm(builtInLed_value));
  digitalWrite(EXT_LED1_PIN, softPwm.pwm(extLed1_value));
  digitalWrite(EXT_LED2_PIN, softPwm.pwm(extLed2_value));

  digitalWrite(PS2_DATA_PIN, softPwm.pwm(ps2Data_value));
  digitalWrite(PS2_CLOCK_PIN, softPwm.pwm(ps2Clock_value));

  gameport.setButtonStates(
      softPwm.pwm(joyButton1_value), softPwm.pwm(joyButton2_value),
      softPwm.pwm(joyButton3_value), softPwm.pwm(joyButton4_value));

  timer.tick();
  if (timer.getElapsedMillis() > 10) {
    gameport.setAxes(joyAxis1_value, joyAxis2_value, joyAxis3_value,
                     joyAxis4_value);
    incrementValues();
    timer.reset();
  }
}
