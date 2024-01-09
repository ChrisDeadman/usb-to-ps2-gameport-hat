#include <Arduino.h>

#include "Config.h"
#include "SoftPWM.h"
SoftPWM soft_pwm;

#include "Watchdog.h"
Watchdog watchdog;

#include <Usb.h>
#include <usbhub.h>

#include "HIDMouseController.h"
#include "JoystickManager.h"
USBHost usb;
USBHub usb_hub(&usb);  // so we can support controllers connected via hubs
HIDBoot<HID_PROTOCOL_MOUSE> hid_mouse_driver(&usb);
HIDMouseController usb_mouse(&hid_mouse_driver);
HIDMouseControllerState usb_mouse_state;
JoystickManager joystick_manager(&usb);
JoystickState joystick_states[2];

#include "PS2Mouse.h"
#include "PS2Port.h"
PS2Port ps2_mouse_port(PS2_CLOCK_PIN, PS2_DATA_PIN, EXT_LED1_PIN);
PS2Mouse ps2_mouse(&ps2_mouse_port);
PS2MouseState ps2_mouse_state;

#include "Gameport.h"
Gameport gameport(POT1_CS_PIN, JOY_BUTTON1_PIN, JOY_BUTTON2_PIN,
                  JOY_BUTTON3_PIN, JOY_BUTTON4_PIN);
JoystickState gameport_state;

#include "SetupMode.h"
SetupMode setup_mode(&gameport_state);

#include "Logging.h"
Logging logging(&ps2_mouse, &joystick_manager, &setup_mode);

inline bool update_joystick_states(uint8_t* const num_joy_devices);
inline void update_gameport_state(uint8_t num_joy_devices);
inline bool update_usb_mouse_state();
inline void update_ps2_mouse_state();

void setup() {
  digitalWrite(EXT_LED1_PIN, HIGH);
  digitalWrite(EXT_LED2_PIN, HIGH);
  pinMode(EXT_LED1_PIN, OUTPUT);
  pinMode(EXT_LED2_PIN, OUTPUT);
  usb.Init();
  usb.Task();
  PS2Port::init();
  ps2_mouse.init();
  ps2_mouse.reset();
  gameport.init();
  logging.init();
  watchdog.init();
}

void loop() {
  watchdog.reset();

  // handle PS/2 tasks
  ps2_mouse.task();

  // don't do anything if PS/2 is busy
  if (ps2_mouse.is_busy()) {
    return;
  }

  // NOTE1: There seems to be some interrupt nesting issues between timer
  // interrupts and USB (freeze causes watchdog reset), so IRQs are
  // disabled here during USB operation.
  // NOTE2: lower USB_XFER_TIMEOUT (e.g. to 500) in UsbCore.h,
  // otherwise this may take longer than the watchdog timeout.
  PS2Port::disable_clock_irq();
  usb.Task();      // handle usb task
  logging.task();  // handle logging task
  PS2Port::enable_clock_irq();

  // update PS/2 mouse state
  if (update_usb_mouse_state()) {
    update_ps2_mouse_state();
  }

  // update gameport state
  uint8_t num_joy_devices;
  if (update_joystick_states(&num_joy_devices)) {
    update_gameport_state(num_joy_devices);
  }

  // handle setup task
  setup_mode.task();

  // EXT_LED2 indicates button press or highest axis value
  if (!setup_mode.in_setup_mode) {
    bool button_pressed = false;
    for (uint8_t button = 0; button < 6; button++) {
      button_pressed |= gameport_state.buttons[button];
    }

    uint8_t highest_axis_value = 0;
    for (uint8_t axis = 0; axis < 4; axis++) {
      uint8_t axisValue = abs((int16_t)gameport_state.axes[axis] - 0x80);
      if (axisValue > highest_axis_value) {
        highest_axis_value = axisValue;
      }
    }

    if (button_pressed) {
      digitalWrite(EXT_LED2_PIN, LOW);
    } else {
      digitalWrite(EXT_LED2_PIN, !soft_pwm.pwm(highest_axis_value));
    }
  }
}

inline bool update_joystick_states(uint8_t* const num_joy_devices) {
  bool updated = false;
  *num_joy_devices = min(joystick_manager.getNumConnectedDevices(), 2);
  for (uint8_t idx = 0; idx < *num_joy_devices; idx++) {
    JoystickState deviceState = joystick_manager.getControllerState(idx);
    updated |=
        deviceState.version_counter != joystick_states[idx].version_counter;
    joystick_states[idx] = deviceState;
  }
  return updated;
}

inline void update_gameport_state(uint8_t num_joy_devices) {
  gameport_state.buttons[0] = joystick_states[0].buttons[0];
  gameport_state.buttons[1] = joystick_states[0].buttons[1];
  gameport_state.buttons[4] = joystick_states[0].buttons[4];
  gameport_state.buttons[5] = joystick_states[0].buttons[5];
  gameport_state.axes[0] = joystick_states[0].axes[0];
  gameport_state.axes[1] = joystick_states[0].axes[1];
  if (num_joy_devices < 2) {
    gameport_state.buttons[2] = joystick_states[0].buttons[2];
    gameport_state.buttons[3] = joystick_states[0].buttons[3];
    if (setup_mode.swap_joy_axis_3_and_4) {
      gameport_state.axes[2] = joystick_states[0].axes[3];
      gameport_state.axes[3] = joystick_states[0].axes[2];
    } else {
      gameport_state.axes[2] = joystick_states[0].axes[2];
      gameport_state.axes[3] = joystick_states[0].axes[3];
    }
  } else {
    gameport_state.buttons[2] = joystick_states[1].buttons[0];
    gameport_state.buttons[3] = joystick_states[1].buttons[1];
    gameport_state.axes[2] = joystick_states[1].axes[0];
    gameport_state.axes[3] = joystick_states[1].axes[1];
  }
  gameport.setAxes(gameport_state.axes[0], gameport_state.axes[1],
                   gameport_state.axes[2], gameport_state.axes[3]);
  gameport.setButtons(gameport_state.buttons[0], gameport_state.buttons[1],
                      gameport_state.buttons[2], gameport_state.buttons[3]);
}

inline bool update_usb_mouse_state() {
  uint8_t prevVersionCounter = usb_mouse_state.version_counter;
  usb_mouse_state = usb_mouse.popState();
  return usb_mouse_state.version_counter != prevVersionCounter;
}

inline void update_ps2_mouse_state() {
  ps2_mouse_state.d_x = usb_mouse_state.d_x;
  ps2_mouse_state.d_y = -usb_mouse_state.d_y;          // y is inverted
  ps2_mouse_state.d_wheel = -usb_mouse_state.d_wheel;  // wheel is inverted
  ps2_mouse_state.button1 = usb_mouse_state.button1;
  ps2_mouse_state.button2 = usb_mouse_state.button2;
  ps2_mouse_state.button3 = usb_mouse_state.button3;
  ps2_mouse_state.button4 = usb_mouse_state.button4;
  ps2_mouse_state.button5 = usb_mouse_state.button5;
  ps2_mouse.update_state(&ps2_mouse_state);
}
