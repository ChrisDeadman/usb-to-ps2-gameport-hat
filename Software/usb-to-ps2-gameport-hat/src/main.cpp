#include <Arduino.h>

#include "Config.h"
#include "SoftPWM.h"
SoftPWM soft_pwm;

#include "Watchdog.h"
Watchdog watchdog;

#include <Usb.h>
#include <usbhub.h>

#include "HIDMouseKeyboardController.h"
#include "JoystickManager.h"
USBHost usb;
USBHub usb_hub(&usb);  // so we can support controllers connected via hubs
HIDBoot<HID_PROTOCOL_MOUSE | HID_PROTOCOL_KEYBOARD> hid_mouse_keyboard_driver(
    &usb);
HIDMouseKeyboardController usb_mouse_keyboard(&hid_mouse_keyboard_driver);
HIDBoot<HID_PROTOCOL_MOUSE> hid_mouse_driver(&usb);
HIDMouseController usb_mouse(&hid_mouse_driver);
HIDMouseState usb_mouse_state[2];
JoystickManager joystick_manager(&usb);
JoystickState joystick_states[2];
HIDBoot<HID_PROTOCOL_KEYBOARD> hid_keyboard_driver(&usb);
HIDKeyboardController usb_keyboard(&hid_keyboard_driver);

#include "PS2Keyboard.h"
#include "PS2Mouse.h"
#include "PS2Port.h"
PS2Port ps2_keyboard_port(PS2_2_CLOCK_PIN, PS2_2_DATA_PIN, EXT_LED1_PIN);
PS2Keyboard ps2_keyboard(&ps2_keyboard_port);
PS2Port ps2_mouse_port(PS2_1_CLOCK_PIN, PS2_1_DATA_PIN, EXT_LED1_PIN);
PS2Mouse ps2_mouse(&ps2_mouse_port);
PS2MouseState ps2_mouse_state;

#include "Gameport.h"
Gameport gameport(POT1_CS_PIN, JOY_BUTTON1_PIN, JOY_BUTTON2_PIN,
                  JOY_BUTTON3_PIN, JOY_BUTTON4_PIN);
JoystickState gameport_state;

#include "SetupMode.h"
SetupMode setup_mode(&gameport_state);

#include "Logging.h"
Logging logging(&usb_mouse_keyboard, &usb_keyboard, &usb_mouse,
                &joystick_manager, &ps2_keyboard, &ps2_mouse, &setup_mode);

inline void sync_keyboard_state();
inline void sync_mouse_state();
inline void sync_gameport_state();

void setup() {
  digitalWrite(EXT_LED1_PIN, HIGH);
  digitalWrite(EXT_LED2_PIN, HIGH);
  pinMode(EXT_LED1_PIN, OUTPUT);
  pinMode(EXT_LED2_PIN, OUTPUT);
  usb.Init();
  delay(2000);  // wait until USB devices are ready
  usb.Task();
  PS2Port::init();
  ps2_keyboard.init();
  ps2_keyboard.reset();
  ps2_mouse.init();
  ps2_mouse.reset();
  gameport.init();
  logging.init();
  watchdog.init();
}

void loop() {
  watchdog.reset();

  // handle PS/2 tasks
  ps2_keyboard.task();
  ps2_mouse.task();

  // don't do anything if PS/2 is busy
  if (ps2_keyboard.is_busy() || ps2_mouse.is_busy()) {
    return;
  }

  // NOTE1: There seems to be some interrupt nesting issues between timer
  // interrupts and USB (freeze causes watchdog reset), so IRQs are
  // disabled here during USB operation.
  // NOTE2: lower USB_XFER_TIMEOUT (e.g. to 1000) in UsbCore.h,
  // otherwise this may take longer than the watchdog timeout.
  PS2Port::disable_clock_irq();
  //
  usb.Task();             // handle usb task
  logging.task();         // handle logging task
  sync_keyboard_state();  // sync states
  sync_mouse_state();
  sync_gameport_state();
  //
  PS2Port::enable_clock_irq();

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

inline void sync_keyboard_state() {
  usb_mouse_keyboard.set_led_state(ps2_keyboard.get_led_state());
  usb_keyboard.set_led_state(ps2_keyboard.get_led_state());

  while (true) {
    KeyboardCodes make = usb_mouse_keyboard.deq_make();
    if (make == KeyboardCodes::NoKey) {
      break;
    }
    ps2_keyboard.enq_make(make);
  }

  while (true) {
    KeyboardCodes make = usb_keyboard.deq_make();
    if (make == KeyboardCodes::NoKey) {
      break;
    }
    ps2_keyboard.enq_make(make);
  }

  while (true) {
    KeyboardCodes brk = usb_mouse_keyboard.deq_brk();
    if (brk == KeyboardCodes::NoKey) {
      break;
    }
    ps2_keyboard.enq_brk(brk);
  }

  while (true) {
    KeyboardCodes brk = usb_keyboard.deq_brk();
    if (brk == KeyboardCodes::NoKey) {
      break;
    }
    ps2_keyboard.enq_brk(brk);
  }
}

inline void sync_mouse_state() {
  uint8_t prev_version_counter[2];
  prev_version_counter[0] = usb_mouse_state[0].version_counter;
  prev_version_counter[1] = usb_mouse_state[1].version_counter;
  usb_mouse_state[0] = usb_mouse_keyboard.pop_state();
  usb_mouse_state[1] = usb_mouse.pop_state();
  if ((usb_mouse_state[0].version_counter == prev_version_counter[0]) &&
      (usb_mouse_state[1].version_counter == prev_version_counter[1])) {
    return;
  }

  ps2_mouse_state.d_x = usb_mouse_state[0].d_x + usb_mouse_state[1].d_x;
  // y is inverted
  ps2_mouse_state.d_y = -(usb_mouse_state[0].d_y + usb_mouse_state[1].d_y);
  // wheel is inverted
  ps2_mouse_state.d_wheel =
      -(usb_mouse_state[0].d_wheel + usb_mouse_state[1].d_wheel);
  ps2_mouse_state.button1 =
      usb_mouse_state[0].button1 | usb_mouse_state[1].button1;
  ps2_mouse_state.button2 =
      usb_mouse_state[0].button2 | usb_mouse_state[1].button2;
  ps2_mouse_state.button3 =
      usb_mouse_state[0].button3 | usb_mouse_state[1].button3;
  ps2_mouse_state.button4 =
      usb_mouse_state[0].button4 | usb_mouse_state[1].button4;
  ps2_mouse_state.button5 =
      usb_mouse_state[0].button5 | usb_mouse_state[1].button5;
  ps2_mouse.update_state(&ps2_mouse_state);
}

inline void sync_gameport_state() {
  bool changed = false;
  uint8_t num_joy_devices = min(joystick_manager.getNumConnectedDevices(), 2);
  for (uint8_t idx = 0; idx < num_joy_devices; idx++) {
    JoystickState deviceState = joystick_manager.getControllerState(idx);
    changed |=
        deviceState.version_counter != joystick_states[idx].version_counter;
    joystick_states[idx] = deviceState;
  }
  if (!changed) {
    return;
  }

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
