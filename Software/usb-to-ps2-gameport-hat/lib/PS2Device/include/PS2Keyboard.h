#ifndef _PS2_KEYBOARD_H_
#define _PS2_KEYBOARD_H_

#include "../../Common/KeyboardCodes.h"
#include "../../Common/KeyboardLeds.h"
#include "CircularBuffer.h"
#include "PS2Device.h"
#include "SoftTimer.h"

#define PS2_KEYBOARD_KRO 6

class PS2Keyboard : public PS2Device {
 private:
  const uint8_t DEFAULT_TYPEMATIC_DELAY = 0x01;
  const uint8_t DEFAULT_TYPEMATIC_RATE = 0x0B;
  const uint8_t SCAN_CODE_SET = 0x41;  // Set 2

  const uint8_t DEVICE_ID[2] = {0xAB, 0x83};

  uint8_t send_buffer[8];
  uint8_t send_buffer_idx = 0;
  uint8_t send_buffer_len = 0;

  uint8_t active_command;
  KeyboardCodes last_keycode;
  SoftTimer typematic_delay_timer;
  SoftTimer typematic_rate_timer;

  bool enabled;
  uint8_t typematic_delay;
  uint8_t typematic_rate;

  KeyboardLeds led_state;
  CircularBuffer<KeyboardCodes, PS2_KEYBOARD_KRO> make_buffer;
  CircularBuffer<KeyboardCodes, PS2_KEYBOARD_KRO> brk_buffer;

 public:
  PS2Keyboard(PS2Port* const port);

  /**
   * Applies default values to all settings.
   */
  void set_defaults();

  /**
   * Returns the current LED state.
   */
  KeyboardLeds get_led_state();

  /**
   * Applies default values and sends a BAT_OK response.
   *
   * If send_ack is set, ACK_CODE is sent before the response.
   */
  void reset(bool send_ack = false);

  /**
   * Enqueues a make code.
   */
  void enq_make(KeyboardCodes code);

  /**
   * Enqueues a break code.
   */
  void enq_brk(KeyboardCodes code);

  /**
   * PS/2 main task.
   *
   * This should be called periodically in the loop() function.
   */
  void task();

 private:
  void send_toHost(const uint8_t* data, uint8_t len);
  void handle_active_command(uint8_t data_byte);
  void handle_new_command(uint8_t data_byte);
};

#endif  //_PS2_KEYBOARD_H_
