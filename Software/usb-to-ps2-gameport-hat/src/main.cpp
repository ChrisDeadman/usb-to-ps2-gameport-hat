#include <Arduino.h>

#include "Config.h"
#include "SoftPWM.h"
#include "SoftTimer.h"
SoftPWM soft_pwm;
SoftTimer init_timer;

#include "Watchdog.h"
Watchdog watchdog;

#include <Usb.h>
#include <usbhub.h>

#include "HIDMouseKeyboardController.h"
USBHost usb;
USBHub usb_hub(&usb);  // so we can support controllers connected via hubs
HIDBoot<HID_PROTOCOL_MOUSE | HID_PROTOCOL_KEYBOARD> hid_mouse_keyboard_driver(&usb);
HIDMouseKeyboardController usb_mouse_keyboard(&hid_mouse_keyboard_driver);
HIDBoot<HID_PROTOCOL_MOUSE> hid_mouse_driver(&usb);
HIDMouseController usb_mouse(&hid_mouse_driver);
HIDBoot<HID_PROTOCOL_KEYBOARD> hid_keyboard_driver(&usb);
HIDKeyboardController usb_keyboard(&hid_keyboard_driver);

#include <XBOXRECV.h>

#include "HIDJoystickController.h"
#include "HIDJoystickControllerMapper.h"
#include "XBOXRECVMapper.h"
// Joystick drivers
XBOXRECV xbox_recv(&usb);
HIDJoystickController hid_joy_controller_1(&usb);
HIDJoystickController hid_joy_controller_2(&usb);
// Joystick mappers
XBOXRECVMapper xbox_recv_mapper(&xbox_recv);
HIDJoystickControllerMapper hid_joy_controller_1_mapper(&hid_joy_controller_1);
HIDJoystickControllerMapper hid_joy_controller_2_mapper(&hid_joy_controller_2);
IJoystickDriverMapper* joy_mappers[] = {
    &xbox_recv_mapper,
    &hid_joy_controller_1_mapper,
    &hid_joy_controller_2_mapper,
};
const uint8_t num_joy_mappers = sizeof(joy_mappers) / sizeof(IJoystickDriverMapper*);

#include "PS2Keyboard.h"
#include "PS2Mouse.h"
#include "PS2Port.h"
PS2Port ps2_keyboard_port(PS2_2_CLOCK_PIN, PS2_2_DATA_PIN);
PS2Keyboard ps2_keyboard(&ps2_keyboard_port);
PS2Port ps2_mouse_port(PS2_1_CLOCK_PIN, PS2_1_DATA_PIN);
PS2Mouse ps2_mouse(&ps2_mouse_port);
MouseState ps2_mouse_state;

#include "Gameport.h"
Gameport gameport(POT1_CS_PIN, JOY_BUTTON1_PIN, JOY_BUTTON2_PIN, JOY_BUTTON3_PIN,
                  JOY_BUTTON4_PIN);

#include "VirtualJoystick.h"
#include "VirtualKeyboard.h"
#include "VirtualMouse.h"
// Virtual Devices
VirtualKeyboard virtual_keyboard;
VirtualMouse virtual_mouse;
VirtualJoystick virtual_joystick;
MouseState mouse_state;
JoystickState joy_state;
uint8_t num_joys_connected = 0;

#include "SetupMode.h"
SetupMode setup_mode(&virtual_keyboard, &joy_state);

#include "Logging.h"
Logging logging(&usb_mouse_keyboard, &usb_keyboard, &usb_mouse, &mouse_state,
                &joy_state, &num_joys_connected, &ps2_keyboard, &ps2_mouse,
                &setup_mode);

static void sync_usb_keyboard_state();
static void sync_usb_mouse_state();
static void sync_usb_joystick_state();

static void sync_ps2_keyboard_state();
static void sync_ps2_mouse_state();
static void sync_gameport_state();

#include "DeviceEmulation.h"
static void handle_device_emulation();

static void update_debug_led_state();

void setup() {
  digitalWrite(EXT_LED1_PIN, HIGH);
  digitalWrite(EXT_LED2_PIN, HIGH);
  pinMode(EXT_LED1_PIN, OUTPUT);
  pinMode(EXT_LED2_PIN, OUTPUT);
  usb.Init();
  PS2Port::init();
  ps2_keyboard.init();
  ps2_mouse.init();
  gameport.init();
  logging.init();
  watchdog.init();
  ps2_keyboard.reset();
  ps2_mouse.reset();
  init_timer.reset();
}

void loop() {
  watchdog.reset();

  // handle PS/2 tasks
  ps2_keyboard.task();
  ps2_mouse.task();

  // check if PS/2 is busy
  bool ps2_busy = ps2_keyboard.is_busy() || ps2_mouse.is_busy();

  // give host some time to respond before we initialize USB devices
  if (init_timer.getElapsedMillis() < PS2_INIT_TIME) {
    init_timer.tick();
    if (ps2_busy) init_timer.reset();
    return;
  }

  // Only if PS/2 is not busy
  if (!ps2_busy) {
    // cannot handle PS/2 while USB is working
    PS2Port::disable_clock_irq();

    // synchronize input devices
    // NOTE: lower USB_XFER_TIMEOUT (e.g. to 1000) in UsbCore.h,
    // otherwise this may take longer than the watchdog timeout.
    usb.Task();
    sync_usb_keyboard_state();
    sync_usb_mouse_state();
    sync_usb_joystick_state();

    // handle device emulation
    if (!setup_mode.in_setup_mode) {
      handle_device_emulation();
    }

    // get input states
    mouse_state = virtual_mouse.pop_state();
    joy_state = virtual_joystick.pop_state();

    PS2Port::enable_clock_irq();
  }

  // handle setup mode
  setup_mode.task();

  // synchronize output devices
  sync_ps2_keyboard_state();
  sync_ps2_mouse_state();
  sync_gameport_state();

  // update LED state if not in setup mode
  if (!setup_mode.in_setup_mode) {
    update_debug_led_state();
  }

  // handle logging
  logging.task();
}

static void sync_usb_keyboard_state() {
  while (true) {
    KeyboardCodes make = usb_keyboard.deq_make();
    if (make == NoKey) {
      make = usb_mouse_keyboard.deq_make();
      if (make == NoKey) {
        break;
      }
    }
    virtual_keyboard.enq_make(make);
  }

  while (true) {
    KeyboardCodes brk = usb_keyboard.deq_brk();
    if (brk == NoKey) {
      brk = usb_mouse_keyboard.deq_brk();
      if (brk == NoKey) {
        break;
      }
    }
    virtual_keyboard.enq_brk(brk);
  }

  virtual_keyboard.pop_modifier_state();
  virtual_keyboard.update_modifier_state(usb_keyboard.get_modifier_state());
  virtual_keyboard.update_modifier_state(usb_mouse_keyboard.get_modifier_state());

  KeyboardLeds led_state = virtual_keyboard.pop_led_state();
  usb_keyboard.set_led_state(led_state);
  usb_mouse_keyboard.set_led_state(led_state);
}

static void sync_usb_mouse_state() {
  MouseState device_state;

  device_state = usb_mouse_keyboard.pop_state();
  if (device_state.changed) {
    virtual_mouse.update_state(&device_state);
  }

  device_state = usb_mouse.pop_state();
  if (device_state.changed) {
    virtual_mouse.update_state(&device_state);
  }
}

static void sync_usb_joystick_state() {
  JoystickState device_state;

  num_joys_connected = 0;

  for (uint8_t mapper_idx = 0; mapper_idx < num_joy_mappers; mapper_idx++) {
    uint8_t num_devices = joy_mappers[mapper_idx]->get_num_connected_devices();
    for (uint8_t device_idx = 0; device_idx < num_devices; device_idx++) {
      num_joys_connected += 1;
      bool is_player_1 = num_joys_connected < 2;
      device_state = joy_mappers[mapper_idx]->pop_state(device_idx);
      virtual_joystick.update_state(&device_state, is_player_1,
                                    setup_mode.swap_joy_axis_3_and_4);
    }
  }
}

static void sync_ps2_keyboard_state() {
  while (true) {
    KeyboardCodes make = virtual_keyboard.deq_make();
    if (make == NoKey) {
      break;
    }
    ps2_keyboard.enq_make(make);
  }

  while (true) {
    KeyboardCodes brk = virtual_keyboard.deq_brk();
    if (brk == NoKey) {
      break;
    }
    ps2_keyboard.enq_brk(brk);
  }

  if (!setup_mode.in_setup_mode) {
    virtual_keyboard.update_led_state(ps2_keyboard.get_led_state());
  }
}

static void sync_ps2_mouse_state() {
  if (mouse_state.changed) {
    ps2_mouse.update_state(&mouse_state);
  }
}

static void sync_gameport_state() {
  if (joy_state.changed) {
    gameport.setAxes(
        AXIS_TO_POT_VALUE(joy_state.axes[0]), AXIS_TO_POT_VALUE(joy_state.axes[1]),
        AXIS_TO_POT_VALUE(joy_state.axes[2]), AXIS_TO_POT_VALUE(joy_state.axes[3]));
    gameport.setButtons(joy_state.buttons[0], joy_state.buttons[1],
                        joy_state.buttons[2], joy_state.buttons[3]);
  }
}

static void handle_device_emulation() {
  MouseState new_mouse_state = virtual_mouse.pop_state();
  JoystickState new_joy_state = virtual_joystick.pop_state();

  // keep previous input states if not changed
  if (!new_mouse_state.changed) {
    new_mouse_state = mouse_state;
    new_mouse_state.changed = false;
  }
  if (!new_joy_state.changed) {
    new_joy_state = joy_state;
    joy_state.changed = false;
  }

  // push back virtual states
  virtual_mouse.update_state(&new_mouse_state);
  virtual_joystick.update_state(&new_joy_state, true, false);

  // handle all emulation modes
  switch (setup_mode.emu_mode) {
    case EmuModeJoyKeyb:
      joy_emulate_keyboard(&virtual_keyboard, &joy_state, &new_joy_state);
      break;
    case EmuModeJoyMouse:
      joy_emulate_mouse(&virtual_mouse, &new_joy_state);
      break;
    case EmuModeKeybJoy:
      keyboard_emulate_joy(&virtual_joystick, &virtual_keyboard);
      break;
    case EmuModeKeybMouse:
      keyboard_emulate_mouse(&virtual_mouse, &virtual_keyboard);
      break;
    case EmuModeMouseJoy:
      mouse_emulate_joy(&virtual_joystick, &new_mouse_state);
      break;
    default:
      break;
  }
}

static void update_debug_led_state() {
  // EXT_LED1 indicates PS/2 status
  digitalWrite(
      EXT_LED1_PIN,
      (ps2_keyboard_port.clock_enabled || ps2_keyboard_port.clock_inhibited ||
       ps2_mouse_port.clock_enabled || ps2_mouse_port.clock_inhibited)
          ? LOW
          : HIGH);

  // EXT_LED2 indicates button press
  bool button_pressed = false;
  for (uint8_t button = 0; button < JoystickState::NUM_BUTTONS; button++) {
    button_pressed |= joy_state.buttons[button];
  }
  if (button_pressed) {
    digitalWrite(EXT_LED2_PIN, LOW);
  } else {
    // or highest axis value
    uint8_t highest_axis_value = 0;
    for (uint8_t axis = 0; axis < JoystickState::NUM_AXES; axis++) {
      uint8_t axisValue = abs((int16_t)joy_state.axes[axis] - 0x80);
      if (axisValue > highest_axis_value) {
        highest_axis_value = axisValue;
      }
    }
    digitalWrite(EXT_LED2_PIN, !soft_pwm.pwm(highest_axis_value >> 1));
  }
}
