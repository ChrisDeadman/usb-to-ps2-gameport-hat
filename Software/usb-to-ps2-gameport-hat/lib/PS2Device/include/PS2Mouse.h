#ifndef _PS2_MOUSE_H_
#define _PS2_MOUSE_H_

#include "CircularBuffer.h"
#include "MouseState.h"
#include "PS2Device.h"
#include "SoftTimer.h"

class PS2Mouse : public PS2Device {
 private:
  const uint8_t DEFAULT_SAMPLE_RATE = 100;
  const uint8_t DEFAULT_RESOLUTION = 4;
  const uint8_t MAX_RESOLUTION = 8;
  const bool DEFAULT_SCALING_2X1 = false;

  const uint8_t DEVICE_ID_MOUSE_STD = 0x00;
  const uint8_t DEVICE_ID_MOUSE_WHEEL = 0x03;
  const uint8_t DEVICE_ID_MOUSE_WHEEL_5BUTTONS = 0x04;

  uint8_t send_buffer[5];
  uint8_t send_buffer_idx = 0;
  uint8_t send_buffer_len = 0;

  CircularBuffer<uint8_t, 3> sample_rate_history;
  SoftTimer sample_timer;

  uint8_t device_id;
  uint8_t active_command;
  bool streaming_mode;
  bool data_reporting;
  uint8_t sample_rate;
  uint8_t resolution;
  bool scaling_2x1;

  MouseState state;
  bool state_changed;

 public:
  PS2Mouse(PS2Port* const port);

  /**
   * Returns the current device-id.
   */
  uint8_t get_device_id();

  /**
   * Applies default values to all settings.
   */
  void set_defaults();

  /**
   * Applies default values and sends a BAT_OK response.
   *
   * If send_ack is set, ACK_CODE is sent before the response.
   */
  void reset(bool send_ack = false);

  /**
   * Updates the mouse state from an external source.
   */
  void update_state(MouseState const* const new_state);

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
  uint8_t build_status_packet(uint8_t* packet);
  uint8_t build_movement_packet(boolean use_2x1_scaling, uint8_t* packet);
  uint8_t apply_2x1_scaling(uint8_t movement);
};

#endif  //_PS2_MOUSE_H_
