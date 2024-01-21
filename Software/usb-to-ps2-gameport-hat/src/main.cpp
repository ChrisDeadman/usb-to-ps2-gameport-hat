#include <Arduino.h>

#include "Config.h"
#include "SoftPWM.h"
SoftPWM soft_pwm;

#include "Watchdog.h"
Watchdog watchdog;

#include <Usb.h>
#include <usbhub.h>

#include "HIDKeyboardCombiner.h"
#include "HIDMouseCombiner.h"
#include "HIDMouseKeyboardController.h"
#include "JoystickManager.h"
USBHost usb;
USBHub usb_hub(&usb);  // so we can support controllers connected via hubs
HIDBoot<HID_PROTOCOL_MOUSE | HID_PROTOCOL_KEYBOARD> hid_mouse_keyboard_driver(&usb);
HIDMouseKeyboardController usb_mouse_keyboard(&hid_mouse_keyboard_driver);
HIDBoot<HID_PROTOCOL_MOUSE> hid_mouse_driver(&usb);
HIDMouseController usb_mouse(&hid_mouse_driver);
HIDMouseCombiner combined_mouse(&usb_mouse_keyboard, &usb_mouse);
HIDMouseState mouse_state;
HIDBoot<HID_PROTOCOL_KEYBOARD> hid_keyboard_driver(&usb);
HIDKeyboardController usb_keyboard(&hid_keyboard_driver);
HIDKeyboardCombiner combined_keyboard(&usb_mouse_keyboard, &usb_keyboard);
JoystickManager joystick_manager(&usb);
JoystickState joystick_states[2];

#include "PS2Keyboard.h"
#include "PS2Mouse.h"
#include "PS2Port.h"
PS2Port ps2_keyboard_port(PS2_2_CLOCK_PIN, PS2_2_DATA_PIN);
PS2Keyboard ps2_keyboard(&ps2_keyboard_port);
PS2Port ps2_mouse_port(PS2_1_CLOCK_PIN, PS2_1_DATA_PIN);
PS2Mouse ps2_mouse(&ps2_mouse_port);
PS2MouseState ps2_mouse_state;

#include "Gameport.h"
Gameport gameport(POT1_CS_PIN, JOY_BUTTON1_PIN, JOY_BUTTON2_PIN, JOY_BUTTON3_PIN,
                  JOY_BUTTON4_PIN);
JoystickState gameport_state;

#include "SetupMode.h"
SetupMode setup_mode(&combined_keyboard, &gameport_state);

#include "Logging.h"
Logging logging(&combined_keyboard, &combined_mouse, &joystick_manager,
                &ps2_keyboard, &ps2_mouse, &setup_mode);

static void sync_keyboard_state();
static void sync_mouse_state();
static void sync_gameport_state();
static void update_led_state();

void setup() {
  digitalWrite(EXT_LED1_PIN, HIGH);
  digitalWrite(EXT_LED2_PIN, HIGH);
  pinMode(EXT_LED1_PIN, OUTPUT);
  pinMode(EXT_LED2_PIN, OUTPUT);
  usb.Init();
  delay(1000);  // wait a second for USB devices to be ready
  usb.Task();
  PS2Port::init();
  ps2_keyboard.init();
  ps2_mouse.init();
  gameport.init();
  logging.init();
  watchdog.init();
  ps2_keyboard.reset();
  ps2_mouse.reset();
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

  // NOTE: lower USB_XFER_TIMEOUT (e.g. to 1000) in UsbCore.h,
  // otherwise this may take longer than the watchdog timeout.
  usb.Task();             // handle usb task
  logging.task();         // handle logging task
  setup_mode.task();      // handle setup task
  sync_gameport_state();  // also needed for setup mode

  // don't do any more stuff if in setup mode
  if (setup_mode.in_setup_mode) {
    return;
  }

  sync_keyboard_state();
  sync_mouse_state();
  update_led_state();
}

static void sync_keyboard_state() {
  combined_keyboard.set_led_state(ps2_keyboard.get_led_state());

  while (true) {
    KeyboardCodes make = combined_keyboard.deq_make();
    if (make == KeyboardCodes::NoKey) {
      break;
    }
    ps2_keyboard.enq_make(make);
  }

  while (true) {
    KeyboardCodes brk = combined_keyboard.deq_brk();
    if (brk == KeyboardCodes::NoKey) {
      break;
    }
    ps2_keyboard.enq_brk(brk);
  }
}

static void sync_mouse_state() {
  uint8_t prev_version_counter = mouse_state.version_counter;
  mouse_state = combined_mouse.pop_state();
  if (mouse_state.version_counter == prev_version_counter) {
    return;
  }

  ps2_mouse_state.d_x = mouse_state.d_x;
  ps2_mouse_state.d_y = -mouse_state.d_y;          // y is inverted
  ps2_mouse_state.d_wheel = -mouse_state.d_wheel;  // wheel is inverted
  ps2_mouse_state.button1 = mouse_state.button1;
  ps2_mouse_state.button2 = mouse_state.button2;
  ps2_mouse_state.button3 = mouse_state.button3;
  ps2_mouse_state.button4 = mouse_state.button4;
  ps2_mouse_state.button5 = mouse_state.button5;
  ps2_mouse.update_state(&ps2_mouse_state);
}

static void sync_gameport_state() {
  bool changed = false;
  uint8_t num_joy_devices = min(joystick_manager.getNumConnectedDevices(), 2);
  for (uint8_t idx = 0; idx < num_joy_devices; idx++) {
    JoystickState state = joystick_manager.getControllerState(idx);
    changed |= state.version_counter != joystick_states[idx].version_counter;
    joystick_states[idx] = state;
  }
  if (!changed) {
    return;
  }

  // joy 1 input
  gameport_state.buttons[0] = joystick_states[0].buttons[0];
  gameport_state.buttons[1] = joystick_states[0].buttons[1];
  gameport_state.buttons[2] = joystick_states[0].buttons[2];
  gameport_state.buttons[3] = joystick_states[0].buttons[3];
  gameport_state.buttons[4] = joystick_states[0].buttons[4];
  gameport_state.buttons[5] = joystick_states[0].buttons[5];
  gameport_state.axes[0] = joystick_states[0].axes[0];
  gameport_state.axes[1] = joystick_states[0].axes[1];
  if (setup_mode.swap_joy_axis_3_and_4) {
    gameport_state.axes[2] = joystick_states[0].axes[3];
    gameport_state.axes[3] = joystick_states[0].axes[2];
  } else {
    gameport_state.axes[2] = joystick_states[0].axes[2];
    gameport_state.axes[3] = joystick_states[0].axes[3];
  }

  // combine joy 2+ inputs
  for (uint8_t idx = 1; idx < num_joy_devices; idx++) {
    gameport_state.buttons[2] |= joystick_states[idx].buttons[0];
    gameport_state.buttons[3] |= joystick_states[idx].buttons[1];
    gameport_state.axes[2] = (((int16_t)gameport_state.axes[2] - 0x80) + ((int16_t)joystick_states[idx].axes[0] - 0x80) + 0x80);
    gameport_state.axes[3] = (((int16_t)gameport_state.axes[3] - 0x80) + ((int16_t)joystick_states[idx].axes[1] - 0x80) + 0x80);
  }

  gameport.setAxes(gameport_state.axes[0], gameport_state.axes[1],
                   gameport_state.axes[2], gameport_state.axes[3]);
  gameport.setButtons(gameport_state.buttons[0], gameport_state.buttons[1],
                      gameport_state.buttons[2], gameport_state.buttons[3]);
}

static void update_led_state() {
  // EXT_LED1 indicates PS/2 status
  digitalWrite(
      EXT_LED1_PIN,
      (ps2_keyboard_port.clock_enabled || ps2_keyboard_port.clock_inhibited ||
       ps2_mouse_port.clock_enabled || ps2_mouse_port.clock_inhibited)
          ? LOW
          : HIGH);

  // EXT_LED2 indicates button press
  bool button_pressed = false;
  for (uint8_t button = 0; button < 6; button++) {
    button_pressed |= gameport_state.buttons[button];
  }
  if (button_pressed) {
    digitalWrite(EXT_LED2_PIN, LOW);
  } else {
    // or highest axis value
    uint8_t highest_axis_value = 0;
    for (uint8_t axis = 0; axis < 4; axis++) {
      uint8_t axisValue = abs((int16_t)gameport_state.axes[axis] - 0x80);
      if (axisValue > highest_axis_value) {
        highest_axis_value = axisValue;
      }
    }
    digitalWrite(EXT_LED2_PIN, !soft_pwm.pwm(highest_axis_value));
  }
}
