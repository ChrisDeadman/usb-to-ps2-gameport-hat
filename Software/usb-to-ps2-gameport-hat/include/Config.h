#ifndef _CONFIG_H_
#define _CONFIG_H_

#define Serial Serial1  // use pins D0/D1
#define SERIAL_SPEED 115200

#define PS2_DATA_PIN 2
#define PS2_CLOCK_PIN 3

#define JOY_BUTTON1_PIN 4
#define JOY_BUTTON2_PIN 5
#define JOY_BUTTON3_PIN 6
#define JOY_BUTTON4_PIN 7

// for compatibility
#define SWAP_JOY_AXIS_3_AND_4 true

// axis 3 and 4 are not always linked
#define APPLY_DEADZONE_TO_AXIS_3_AND_4 false

#define EXT_LED1_PIN 8
#define EXT_LED2_PIN 9

#define POT1_CS_PIN 10

#endif  // _CONFIG_H_
