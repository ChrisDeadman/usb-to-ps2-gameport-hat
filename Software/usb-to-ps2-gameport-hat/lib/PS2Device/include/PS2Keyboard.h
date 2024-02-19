#ifndef _PS2_KEYBOARD_H_
#define _PS2_KEYBOARD_H_

#include "KeyBuffer.h"
#include "KeyboardAction.h"
#include "KeyboardLeds.h"
#include "PS2Device.h"
#include "SoftTimer.h"

class PS2Keyboard : public PS2Device {
 private:
  const uint8_t DEFAULT_TYPEMATIC_DELAY = 0x01;
  const uint8_t DEFAULT_TYPEMATIC_RATE = 0x0B;
  const uint8_t SCAN_CODE_SET = 0x41;  // Set 2

  const uint8_t DEVICE_ID[2] = {0xAB, 0x83};

  uint8_t send_buffer[8];
  uint8_t send_buffer_idx;
  uint8_t send_buffer_len;

  uint8_t active_command;
  KeyboardAction last_action;
  SoftTimer typematic_delay_timer;
  SoftTimer typematic_rate_timer;

  bool enabled;
  uint8_t typematic_delay;
  uint8_t typematic_rate;

  KeyboardLeds led_state;
  KeyBuffer key_buffer;

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
   * Enqueues a keyboard action.
   */
  void enq(KeyboardAction kb_action);

  /**
   * PS/2 main task.
   *
   * This should be called periodically in the loop() function.
   */
  void task();

  /**
   * Triggers resending of data upon next clock cycle.
   *
   * This has no effect if no data is currently being sent.
   */
  void resend() override;

 private:
  void send_toHost(const uint8_t* data, uint8_t length);
  void handle_active_command(uint8_t data_byte);
  void handle_new_command(uint8_t data_byte);
};

#endif  //_PS2_KEYBOARD_H_
