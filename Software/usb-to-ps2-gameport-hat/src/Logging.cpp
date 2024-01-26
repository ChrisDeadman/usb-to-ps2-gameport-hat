#include "Logging.h"

#include "Config.h"
#include "GlobalStringBuffer.h"

GlobalStringBuffer* log_buffer = GlobalStringBuffer::alloc(2047);

volatile unsigned long t_current;
volatile unsigned long t_usb_last_received;
volatile unsigned long t_usb_last_sent;
volatile unsigned long t_ps2_last_received;
volatile unsigned long t_ps2_last_sent;

extern "C" char* sbrk(int incr);
int get_free_memory() {
  uint8_t stack_top;
  return &stack_top - reinterpret_cast<uint8_t*>(sbrk(0));
}

/**
 * Callback, captures packets from USB IN.
 */
void usb_data_received(uint8_t const* const data, uint8_t length) {
  t_usb_last_received = t_current;
#ifdef DEBUG
  log_buffer->concat("U<");
  for (uint8_t i = 0; i < length; i++) {
    log_buffer->concat("%02X", data[i]);
  }
  log_buffer->concatln("");
#endif
}

/**
 * Callback, captures packets from USB OUT.
 */
void usb_data_sent(uint8_t const* const data, uint8_t length) {
  t_usb_last_sent = t_current;
#ifdef DEBUG
  log_buffer->concat("U>");
  for (uint8_t i = 0; i < length; i++) {
    log_buffer->concat("%02X", data[i]);
  }
  log_buffer->concatln("");
#endif
}

/**
 * Callback, captures packets from PS2Receiver.
 */
void ps2_data_received(uint8_t pin, uint8_t data, bool valid) {
  t_ps2_last_received = t_current;
#ifdef DEBUG
  log_buffer->concat("P%u< ", pin)
      ->concat("%02X", data)
      ->concatln(valid ? "" : "!!");
#endif
}

/**
 * Callback, captures packets from PS2Sender.
 */
void ps2_data_sent(uint8_t pin, uint8_t data) {
  t_ps2_last_sent = t_current;
#ifdef DEBUG
  log_buffer->concat("P%u> ", pin)->concatln("%02X", data);
#endif
}

static void print_and_flush_buffer() {
  if (log_buffer->length() > 0) {
    Serial.println(log_buffer->get());
    log_buffer->clear();
  }
}

Logging::Logging(HIDMouseKeyboardController* const usb_mouse_keyboard,
                 HIDKeyboardController* const usb_keyboard,
                 HIDMouseController* const usb_mouse, MouseState* const mouse_state,
                 JoystickState* const joystick_state,
                 uint8_t* const num_joys_connected, PS2Keyboard* const ps2_keyboard,
                 PS2Mouse* const ps2_mouse, SetupMode* const setup_mode)
    : usb_mouse_keyboard(usb_mouse_keyboard),
      usb_keyboard(usb_keyboard),
      usb_mouse(usb_mouse),
      mouse_state(mouse_state),
      joy_state(joystick_state),
      num_joys_connected(num_joys_connected),
      ps2_keyboard(ps2_keyboard),
      ps2_mouse(ps2_mouse),
      setup_mode(setup_mode) {}

/**
 * stdout callback
 */
static int serial_write(void* uart, const char* buf, int n) {
  Uart* s = (Uart*)uart;
  return s->write((uint8_t*)buf, n);
}

void Logging::init() {
  Serial.begin(SERIAL_SPEED);
  // redirect host platform debug information to serial port
  stdout = funopen((void*)(&Serial), NULL, serial_write, NULL, NULL);
  setlinebuf(stdout);
}

void Logging::task() {
  t_current = millis();

  // print log if someone writes to our serial input
  if (Serial.available() && Serial.read()) {
    print_and_flush_buffer();
    log_status();
    print_and_flush_buffer();
    // flush input buffer
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
}

void Logging::log_status() {
  log_buffer->concatln("╔═════════════════════════════════╗");
  log_buffer->concatln("║ USB => PS/2 & Gameport HAT V1.4 ║");
  log_buffer->concatln("╠════════════════╦════════════════╝");
  log_buffer->concatln("║ General status ║");
  log_buffer->concatln("╠════════════════╝");
  log_buffer->concat("║free memory: ")->concatln("%d", get_free_memory());
  log_buffer->concatln("╠");
  log_buffer->concat("║time: ")->concatln("%lu", t_current);
  log_buffer->concatln("╠");
  log_buffer->concat("║ext led1: ")
      ->concatln("%u", digitalRead(EXT_LED1_PIN) == LOW);
  log_buffer->concat("║ext led2: ")
      ->concatln("%u", digitalRead(EXT_LED2_PIN) == LOW);
  log_buffer->concatln("╠══════════╗");
  log_buffer->concatln("║ Settings ║");
  log_buffer->concatln("╠══════════╝");
  log_buffer->concat("║swap joy axis 3 and 4: ");
  log_buffer->concatln("%u", (uint8_t)setup_mode->swap_joy_axis_3_and_4);
  log_buffer->concatln("╠════════════╗");
  log_buffer->concatln("║ USB status ║");
  log_buffer->concatln("╠════════════╝");
  log_buffer->concat("║kbd+mouse connected: ")
      ->concatln("%u", usb_mouse_keyboard->is_connected());
  log_buffer->concat("║keyboard connected:  ")
      ->concatln("%u", usb_keyboard->is_connected());
  log_buffer->concat("║mouse connected:     ")
      ->concatln("%u", usb_mouse->is_connected());
  log_buffer->concat("║#joysticks/gamepads: ")->concatln("%u", *num_joys_connected);
  log_buffer->concatln("╠");
  log_buffer->concat("║last received time: ")->concatln("%lu", t_usb_last_received);
  log_buffer->concat("║last sent time:     ")->concatln("%lu", t_usb_last_sent);
  log_buffer->concatln("╠═════════════╗");
  log_buffer->concatln("║ PS/2 status ║");
  log_buffer->concatln("╠═════════════╝");
  log_buffer->concat("║last received time: ")->concatln("%lu", t_ps2_last_received);
  log_buffer->concat("║last sent time:     ")->concatln("%lu", t_ps2_last_sent);
  log_buffer->concatln("╠══════════════════════╗");
  log_buffer->concatln("║ PS/2 keyboard status ║");
  log_buffer->concatln("╠══════════════════════╝");
  log_buffer->concat("║clock: ")
      ->concatln("%u", digitalRead(ps2_keyboard->port->clock_pin));
  log_buffer->concat("║data:  ")
      ->concatln("%u", digitalRead(ps2_keyboard->port->data_pin));
  log_buffer->concatln("╠");
  log_buffer->concat("║last inhibit time:  ")
      ->concatln("%lu", ps2_keyboard->get_time_last_inhibit());
  log_buffer->concat("║last host RTS time: ")
      ->concatln("%lu", ps2_keyboard->get_time_last_host_rts());
  log_buffer->concatln("╠═══════════════════╗");
  log_buffer->concatln("║ PS/2 mouse status ║");
  log_buffer->concatln("╠═══════════════════╝");
  log_buffer->concat("║device id: ")->concatln("%d", ps2_mouse->get_device_id());
  log_buffer->concatln("╠");
  log_buffer->concat("║clock: ")
      ->concatln("%u", digitalRead(ps2_mouse->port->clock_pin));
  log_buffer->concat("║data:  ")
      ->concatln("%u", digitalRead(ps2_mouse->port->data_pin));
  log_buffer->concatln("╠");
  log_buffer->concat("║last inhibit time:  ")
      ->concatln("%lu", ps2_mouse->get_time_last_inhibit());
  log_buffer->concat("║last host RTS time: ")
      ->concatln("%lu", ps2_mouse->get_time_last_host_rts());
  log_buffer->concatln("╠");
  log_buffer->concat("║buttons: ");
  for (uint8_t button = 0; button < MouseState::NUM_BUTTONS; button++) {
    log_buffer->concat("%02X ", mouse_state->buttons[button]);
  }
  log_buffer->concatln("");
  log_buffer->concatln("╠═════════════════╗");
  log_buffer->concatln("║ Joystick status ║");
  log_buffer->concatln("╠═════════════════╝");
  log_buffer->concat("║buttons: ");
  for (uint8_t button = 0; button < JoystickState::NUM_BUTTONS; button++) {
    log_buffer->concat("%02X ", joy_state->buttons[button]);
  }
  log_buffer->concatln("");
  log_buffer->concat("║axes:    ");
  for (uint8_t axis = 0; axis < JoystickState::NUM_AXES; axis++) {
    log_buffer->concat("%02X ", joy_state->axes[axis]);
  }
  log_buffer->concatln("");
  log_buffer->concatln("╚════════════════════╝");
}
