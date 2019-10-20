#include <Arduino.h>
#include "Config.h"

#include "SoftPWM.h"
SoftPWM softPwm;

#include "SoftTimer.h"
SoftTimer setupTimer;
SoftTimer blinkTimer;
SoftTimer inputDelayTimer;

#define BLINK_DURATION 1000
#define SETUP_DELAY 1000
#define INPUT_DELAY 100

#include "Watchdog.h"
Watchdog watchdog;

#include "Deadzone.h"
Deadzone deadzone;

#include <Usb.h>
#include <usbhub.h>
#include "HIDMouseController.h"
#include "JoystickManager.h"
USBHost usb;
USBHub usbHub(&usb);  // so we can support controllers connected via hubs
HIDMouseController usbMouse(&usb);
HIDMouseControllerState usbMouseState;
JoystickManager joystickManager(&usb);
JoystickState joystickStates[2];

#include "PS2Mouse.h"
#include "PS2Port.h"
PS2Port ps2Port(PS2_CLOCK_PIN, PS2_DATA_PIN, EXT_LED1_PIN);
PS2Mouse ps2Mouse(&ps2Port);
PS2MouseState ps2MouseState;

#include "Gameport.h"
Gameport gameport(POT1_CS_PIN, JOY_BUTTON1_PIN, JOY_BUTTON2_PIN,
                  JOY_BUTTON3_PIN, JOY_BUTTON4_PIN);
JoystickState gameportState;

#include "Logging.h"
Logging logging(&ps2Mouse, &joystickManager, &deadzone);

inline float axisToFloat(uint8_t value);
inline uint8_t floatToAxis(float value);
inline void applyDeadzone(uint8_t* const x, uint8_t* const y);
inline bool updateJoystickStates(uint8_t* const numJoyDevices);
inline void updateGameportState(uint8_t numJoyDevices);
inline bool updateUsbMouseState();
inline void updatePS2MouseState();

bool inSetupMode = false;

void setup() {
  digitalWrite(EXT_LED1_PIN, HIGH);
  digitalWrite(EXT_LED2_PIN, HIGH);
  pinMode(EXT_LED1_PIN, OUTPUT);
  pinMode(EXT_LED2_PIN, OUTPUT);
  usb.Init();
  usb.Task();
  ps2Port.init();
  ps2Mouse.reset();
  gameport.init();
  logging.init();
  watchdog.init();
  setupTimer.reset();
  blinkTimer.reset();
  inputDelayTimer.reset();
}

void loop() {
  watchdog.reset();

  ps2Mouse.task();

  // Don't do anything if PS/2 is busy.
  if (ps2Mouse.isBusy()) {
    return;
  }

  // Poll USB device.
  // NOTE1: There seems to be some interrupt nesting issues between timer
  // interrupts and USB (freeze causes watchdog reset), so IRQs are
  // disabled here during USB operation.
  // NOTE2: lower USB_XFER_TIMEOUT (e.g. to 500) in UsbCore.h,
  // otherwise this may take longer than the watchdog's timeout (4s).
  ps2Port.disableClockIrq();
  usb.Task();
  ps2Port.enableClockIrq();

  logging.task();

  setupTimer.tick();
  blinkTimer.tick();
  inputDelayTimer.tick();

  // For everything that needs blinking
  if (blinkTimer.getElapsedMillis() > BLINK_DURATION) {
    blinkTimer.reset();
  }
  bool blinkState = blinkTimer.getElapsedMillis() < (BLINK_DURATION / 2);

  // update PS/2 mouse state
  if (updateUsbMouseState()) {
    updatePS2MouseState();
  }

  // update gameport state
  uint8_t numJoyDevices;
  if (updateJoystickStates(&numJoyDevices)) {
    updateGameportState(numJoyDevices);
  }

  // setup mode is triggered by holding down Button5 + Button6 for a second
  if (!gameportState.buttons[4] || !gameportState.buttons[5]) {
    setupTimer.reset();
  }
  if (setupTimer.getElapsedMillis() > SETUP_DELAY) {
    setupTimer.reset();
    inSetupMode = !inSetupMode;
    digitalWrite(EXT_LED1_PIN, HIGH);
    digitalWrite(EXT_LED2_PIN, HIGH);
  }

  // setup mode
  if (inSetupMode) {
    if (inputDelayTimer.getElapsedMillis() > INPUT_DELAY) {
      if (gameportState.buttons[4] && gameportState.buttons[5]) {
        // do nothing, seems like user wants to exit setup mode
      } else if (gameportState.buttons[4]) {
        deadzone.increase();
      } else if (gameportState.buttons[5]) {
        deadzone.decrease();
      }
      inputDelayTimer.reset();
    }
    digitalWrite(EXT_LED1_PIN, blinkState);
    digitalWrite(EXT_LED2_PIN, !softPwm.pwm(deadzone.getValue() * 0xFF));
  }
  // EXT_LED2 indicates button press or highest axis value
  else {
    bool buttonPressed = false;
    uint8_t highestAxisValue = 0;
    for (uint8_t button = 0; button < 6; button++) {
      buttonPressed |= gameportState.buttons[button];
    }
    for (uint8_t axis = 0; axis < 4; axis++) {
      uint8_t axisValue = abs((int16_t)gameportState.axes[axis] - 0x80);
      if (axisValue > highestAxisValue) {
        highestAxisValue = axisValue;
      }
    }

    if (buttonPressed) {
      digitalWrite(EXT_LED2_PIN, LOW);
    } else {
      digitalWrite(EXT_LED2_PIN, !softPwm.pwm(highestAxisValue));
    }
  }
}

inline float axisToFloat(uint8_t value) { return -1.0f + (value / 127.5f); }

inline uint8_t floatToAxis(float value) {
  return min((value + 1.0f) * 128.0f, 0xFF);
}

inline void applyDeadzone(uint8_t* const x, uint8_t* const y) {
  if (deadzone.isEnabled()) {
    float xFloat = axisToFloat(*x);
    float yFloat = axisToFloat(*y);
    deadzone.apply(&xFloat, &yFloat);
    *x = floatToAxis(xFloat);
    *y = floatToAxis(yFloat);
  }
}

inline bool updateJoystickStates(uint8_t* const numJoyDevices) {
  bool updated = false;
  *numJoyDevices = min(joystickManager.getNumConnectedDevices(), 2);
  for (uint8_t idx = 0; idx < *numJoyDevices; idx++) {
    JoystickState deviceState = joystickManager.getControllerState(idx);
    updated |= deviceState.versionCounter != joystickStates[idx].versionCounter;
    joystickStates[idx] = deviceState;
  }
  return updated;
}

inline void updateGameportState(uint8_t numJoyDevices) {
  gameportState.buttons[0] = joystickStates[0].buttons[0];
  gameportState.buttons[1] = joystickStates[0].buttons[1];
  gameportState.buttons[4] = joystickStates[0].buttons[4];
  gameportState.buttons[5] = joystickStates[0].buttons[5];
  gameportState.axes[0] = joystickStates[0].axes[0];
  gameportState.axes[1] = joystickStates[0].axes[1];
  applyDeadzone(&gameportState.axes[0], &gameportState.axes[1]);
  if (numJoyDevices < 2) {
    gameportState.buttons[2] = joystickStates[0].buttons[2];
    gameportState.buttons[3] = joystickStates[0].buttons[3];
#if SWAP_JOY_AXIS_3_AND_4
    gameportState.axes[2] = joystickStates[0].axes[3];
    gameportState.axes[3] = joystickStates[0].axes[2];
#else
    gameportState.axes[2] = joystickStates[0].axes[2];
    gameportState.axes[3] = joystickStates[0].axes[3];
#endif
#if APPLY_DEADZONE_TO_AXIS_3_AND_4
    applyDeadzone(&gameportState.axes[2], &gameportState.axes[3]);
#endif
  } else {
    gameportState.buttons[2] = joystickStates[1].buttons[0];
    gameportState.buttons[3] = joystickStates[1].buttons[1];
    gameportState.axes[2] = joystickStates[1].axes[0];
    gameportState.axes[3] = joystickStates[1].axes[1];
    applyDeadzone(&gameportState.axes[2], &gameportState.axes[3]);
  }
  gameport.setAxes(gameportState.axes[0], gameportState.axes[1],
                   gameportState.axes[2], gameportState.axes[3]);
  gameport.setButtons(gameportState.buttons[0], gameportState.buttons[1],
                      gameportState.buttons[2], gameportState.buttons[3]);
}

inline bool updateUsbMouseState() {
  uint8_t prevVersionCounter = usbMouseState.versionCounter;
  usbMouseState = usbMouse.popState();
  return usbMouseState.versionCounter != prevVersionCounter;
}

inline void updatePS2MouseState() {
  ps2MouseState.dX = usbMouseState.dX;
  ps2MouseState.dY = -usbMouseState.dY;          // y is inverted
  ps2MouseState.dWheel = -usbMouseState.dWheel;  // wheel is inverted
  ps2MouseState.button1 = usbMouseState.button1;
  ps2MouseState.button2 = usbMouseState.button2;
  ps2MouseState.button3 = usbMouseState.button3;
  ps2MouseState.button4 = usbMouseState.button4;
  ps2MouseState.button5 = usbMouseState.button5;
  ps2Mouse.updateState(&ps2MouseState);
}
