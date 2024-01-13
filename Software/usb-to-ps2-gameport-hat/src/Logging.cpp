#include "Logging.h"

#include "Config.h"
#include "GlobalStringBuffer.h"

GlobalStringBuffer* log_buffer = GlobalStringBuffer::alloc(2048);

volatile unsigned long t_current;
volatile unsigned long t_last_received;
volatile unsigned long t_last_sent;

extern "C" char* sbrk(int incr);
int get_free_memory() {
  uint8_t stack_top;
  return &stack_top - reinterpret_cast<uint8_t*>(sbrk(0));
}

/**
 * Callback, captures packets from PS2Receiver.
 */
void ps2_data_received(uint8_t data_byte, bool data_valid) {
  t_last_received = t_current;
  log_buffer->concat("<= ")
      ->concat("%02X", data_byte)
      ->concatln(data_valid ? "" : "!!");
}

/**
 * Callback, captures packets from PS2Sender.
 */
void ps2_data_sent(uint8_t data_byte) {
  t_last_sent = t_current;
  log_buffer->concat("=> ")->concatln("%02X", data_byte);
}

void print_and_flush_buffer() {
  if (!log_buffer->isEmpty()) {
    Serial.println(log_buffer->get());
    log_buffer->clear();
  }
}

Logging::Logging(HIDKeyboardCombiner* const usb_keyboard,
                 HIDMouseCombiner* const usb_mouse,
                 JoystickManager* const joystick_manager,
                 PS2Keyboard* const ps2_keyboard, PS2Mouse* const ps2_mouse,
                 SetupMode* const setup_mode)
    : usb_keyboard(usb_keyboard),
      usb_mouse(usb_mouse),
      joystick_manager(joystick_manager),
      ps2_keyboard(ps2_keyboard),
      ps2_mouse(ps2_mouse),
      setup_mode(setup_mode) {}

void Logging::init() { Serial.begin(SERIAL_SPEED); }

void Logging::task() {
  t_current = millis();

  // print log if someone writes to our serial input
  if (Serial.available() && Serial.read()) {
    print_and_flush_buffer();
    log_status();
    print_and_flush_buffer();
  }
}

void Logging::log_status() {
  uint8_t num_connected_devices = joystick_manager->getNumConnectedDevices();
  JoystickState joy1_state = joystick_manager->getControllerState(0);
  JoystickState joy2_state = joystick_manager->getControllerState(1);

  log_buffer->concatln("-------------------------------");
  log_buffer->concatln("USB => PS/2 & Gameport HAT V1.4");
  log_buffer->concatln("-------------------------------");
  log_buffer->concatln("General status");
  log_buffer->concatln("--------------");
  log_buffer->concat("free memory: ")->concatln("%d", get_free_memory());
  log_buffer->concat("time: ")->concatln("%lu", t_current);
  log_buffer->concat("ext led1: ")->concatln("%u", digitalRead(EXT_LED1_PIN));
  log_buffer->concat("ext led2: ")->concatln("%u", digitalRead(EXT_LED2_PIN));
  log_buffer->concatln("-----------------");
  log_buffer->concatln("Settings");
  log_buffer->concatln("-----------------");
  log_buffer->concat("swap_joy_axis_3_and_4: ");
  log_buffer->concatln("%u", (uint8_t)setup_mode->swap_joy_axis_3_and_4);
  log_buffer->concatln("-----------------");
  log_buffer->concatln("PS/2 status");
  log_buffer->concatln("-----------------");
  log_buffer->concat("last received time: ")->concatln("%lu", t_last_received);
  log_buffer->concat("last sent time: ")->concatln("%lu", t_last_sent);
  log_buffer->concatln("-----------------");
  log_buffer->concatln("PS/2 keyboard status");
  log_buffer->concatln("-----------------");
  log_buffer->concat("connected: ")
      ->concatln("%u", usb_keyboard->is_connected());
  log_buffer->concat("clock: ")->concatln(
      "%u", digitalRead(ps2_keyboard->port->clock_pin));
  log_buffer->concat("data: ")->concatln(
      "%u", digitalRead(ps2_keyboard->port->data_pin));
  log_buffer->concat("last inhibit time: ")
      ->concatln("%lu", ps2_keyboard->get_time_last_inhibit());
  log_buffer->concat("last host RTS time: ")
      ->concatln("%lu", ps2_keyboard->get_time_last_host_rts());
  log_buffer->concatln("-----------------");
  log_buffer->concatln("PS/2 mouse status");
  log_buffer->concatln("-----------------");
  log_buffer->concat("connected: ")->concatln("%u", usb_mouse->is_connected());
  log_buffer->concat("device id: ")->concatln("%d", ps2_mouse->get_device_id());
  log_buffer->concat("clock: ")->concatln(
      "%u", digitalRead(ps2_mouse->port->clock_pin));
  log_buffer->concat("data: ")->concatln(
      "%u", digitalRead(ps2_mouse->port->data_pin));
  log_buffer->concat("last inhibit time: ")
      ->concatln("%lu", ps2_mouse->get_time_last_inhibit());
  log_buffer->concat("last host RTS time: ")
      ->concatln("%lu", ps2_mouse->get_time_last_host_rts());
  log_buffer->concatln("---------------");
  log_buffer->concatln("Joystick status");
  log_buffer->concatln("---------------");
  log_buffer->concat("connected devices: ")
      ->concatln("%u", num_connected_devices);
  log_buffer->concat("Joy1 buttons: ");
  for (uint8_t button = 0; button < 6; button++) {
    log_buffer->concat("%u ", joy1_state.buttons[button]);
  }
  log_buffer->concatln("");
  log_buffer->concat("Joy2 buttons: ");
  for (uint8_t button = 0; button < 6; button++) {
    log_buffer->concat("%u ", joy2_state.buttons[button]);
  }
  log_buffer->concatln("");
  log_buffer->concat("Joy1 axes: ");
  for (uint8_t axis = 0; axis < 4; axis++) {
    log_buffer->concat("%02X ", joy1_state.axes[axis]);
  }
  log_buffer->concatln("");
  log_buffer->concat("Joy2 axes: ");
  for (uint8_t axis = 0; axis < 4; axis++) {
    log_buffer->concat("%02X ", joy2_state.axes[axis]);
  }
  log_buffer->concatln("");
  log_buffer->concatln("-------------------------------");
}
