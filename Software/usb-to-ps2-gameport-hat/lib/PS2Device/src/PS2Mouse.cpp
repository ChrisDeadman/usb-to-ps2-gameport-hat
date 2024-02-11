#include "PS2Mouse.h"

PS2Mouse::PS2Mouse(PS2Port* port) : PS2Device(port) { set_defaults(); }

uint8_t PS2Mouse::get_device_id() { return device_id; }

void PS2Mouse::set_defaults() {
  device_id = DEVICE_ID_MOUSE_STD;
  streaming_mode = true;
  active_command = 0;
  data_reporting = false;
  sample_rate = DEFAULT_SAMPLE_RATE;
  resolution = DEFAULT_RESOLUTION;
  scaling_2x1 = DEFAULT_SCALING_2X1;
  state_changed = false;
  sample_rate_history.clear();
}

void PS2Mouse::reset(bool send_ack) {
  set_defaults();
  uint8_t packet[3];
  uint8_t idx = 0;
  if (send_ack) {
    packet[idx++] = ACK_CODE;
  }
  packet[idx++] = BAT_OK;
  packet[idx++] = device_id;
  send_toHost(packet, idx);
}

void PS2Mouse::update_state(MouseState const* const new_state) {
  uint16_t d_x_scaled = abs(new_state->d_x);
  uint16_t d_y_scaled = abs(new_state->d_y);

  // assume input is max.resolution
  uint8_t res = resolution;
  while (res < MAX_RESOLUTION) {
    if (d_x_scaled > 1) d_x_scaled >>= 1;
    if (d_y_scaled > 1) d_y_scaled >>= 1;
    res <<= 1;
  }

  state.d_x = (new_state->d_x < 0) ? -d_x_scaled : d_x_scaled;
  state.d_y = (new_state->d_y < 0) ? -d_y_scaled : d_y_scaled;
  state.d_wheel = new_state->d_wheel;
  memcpy(state.buttons, new_state->buttons, MouseState::NUM_BUTTONS);
  state_changed = true;
}

void PS2Mouse::task() {
  // wait until transmission is finished
  if (receiver.is_receiving() || sender.is_sending()) {
    return;
  }

  // don't do anything if inhibited
  if (port->clock_inhibited) {
    return;
  }

  // data received
  if (receiver.has_data()) {
    // stop sending in case we currently are
    send_buffer_idx = send_buffer_len;
    // invoke command handlers
    if (receiver.is_data_valid()) {
      if (active_command) {
        handle_active_command(receiver.pop_data());
      } else {
        handle_new_command(receiver.pop_data());
      }
    }
    return;
  }

  // check if we have anything to send
  if (send_buffer_idx < send_buffer_len) {
    uint8_t dataByte = send_buffer[send_buffer_idx++];
    sender.begin_send(dataByte);
    return;
  }

  // wait until sample time has passed
  sample_timer.tick();
  if (sample_timer.getElapsedMillis() < (1000 / sample_rate)) {
    return;
  }

  // don't send movement data if
  if (!state_changed ||       // there is no change
      !streaming_mode ||      // or streaming mode is disabled
      !data_reporting ||      // or data reporting is disabled
      (active_command != 0))  // or an active command is being processed
  {
    return;
  }

  // send movement data
  uint8_t packet[4];
  uint8_t packet_len = build_movement_packet(scaling_2x1, packet);
  send_toHost(packet, packet_len);
  state_changed = false;
  sample_timer.reset();
}

void PS2Mouse::send_toHost(const uint8_t* data, uint8_t len) {
  for (uint8_t idx = 0; idx < len; idx++) {
    send_buffer[idx] = data[idx];
  }
  send_buffer_idx = 0;
  send_buffer_len = len;
}

void PS2Mouse::handle_active_command(uint8_t data_byte) {
  switch (active_command) {
    // Wrap Mode
    case (0xEE):
      switch (data_byte) {
        // Reset
        case 0xFF:
          reset(true);
          active_command = 0;
          break;
        // Reset Wrap Mode
        case 0xEC:
          send_toHost(&ACK_CODE, 1);
          active_command = 0;
          break;
        // echo every other byte in wrap mode
        default:
          send_toHost(&data_byte, 1);
          break;
      }
      break;
    // Set resolution
    case (0xE8):
      switch (data_byte) {
        // Reset
        case 0xFF:
          reset(true);
          active_command = 0;
          break;
        default:
          resolution = 1 << data_byte;
          if (resolution > MAX_RESOLUTION) resolution = MAX_RESOLUTION;
          send_toHost(&ACK_CODE, 1);
          active_command = 0;
          break;
      }
      break;
    // Set sample rate
    case (0xF3):
      switch (data_byte) {
        // Reset
        case 0xFF:
          reset(true);
          active_command = 0;
          break;
        default:
          sample_rate = data_byte;
          sample_rate_history.enq(sample_rate);
          if (sample_rate_history.is_filled()) {
            // Intellimouse wheel mode
            if (sample_rate_history.get(0) == 200 &&
                sample_rate_history.get(1) == 100 &&
                sample_rate_history.get(2) == 80) {
              device_id = DEVICE_ID_MOUSE_WHEEL;
              sample_rate_history.clear();
            }
            // Intellimouse wheel + 5 button mode
            else if (sample_rate_history.get(0) == 200 &&
                     sample_rate_history.get(1) == 200 &&
                     sample_rate_history.get(2) == 80) {
              device_id = DEVICE_ID_MOUSE_WHEEL_5BUTTONS;
              sample_rate_history.clear();
            }
          }
          send_toHost(&ACK_CODE, 1);
          active_command = 0;
          break;
      }
      break;
    // Unknown active command
    default:
      active_command = 0;
      break;
  }
}

void PS2Mouse::handle_new_command(uint8_t data_byte) {
  uint8_t packet[5];  // prepare packet buffer
  uint8_t packet_len;

  switch (data_byte) {
    // Reset
    case 0xFF:
      reset(true);
      break;
    // Resend
    case 0xFE:
      send_buffer_idx = 0;
      break;
    // Set defaults
    case 0xF6:
      set_defaults();
      send_toHost(&ACK_CODE, 1);
      break;
    // Get Device ID
    case 0xF2:
      packet[0] = ACK_CODE;
      packet[1] = device_id;
      send_toHost(packet, 2);
      break;
    // Set Stream Mode
    case 0xEA:
      streaming_mode = true;
      send_toHost(&ACK_CODE, 1);
      break;
    // Set Remote Mode
    case 0xF0:
      streaming_mode = false;
      send_toHost(&ACK_CODE, 1);
      break;
    // Set Wrap Mode
    case 0xEE:
      active_command = data_byte;
      send_toHost(&ACK_CODE, 1);
      break;
    // Set scaling 1:1
    case 0xE6:
      scaling_2x1 = false;
      send_toHost(&ACK_CODE, 1);
      break;
    // Set scaling 2:1
    case 0xE7:
      scaling_2x1 = true;
      send_toHost(&ACK_CODE, 1);
      break;
    // Set resolution
    case 0xE8:
      active_command = data_byte;
      send_toHost(&ACK_CODE, 1);
      break;
    // Set sample rate
    case 0xF3:
      active_command = data_byte;
      send_toHost(&ACK_CODE, 1);
      break;
    // Enable Data Reporting
    case 0xF4:
      data_reporting = true;
      send_toHost(&ACK_CODE, 1);
      break;
    // Disable Data Reporting
    case 0xF5:
      data_reporting = false;
      send_toHost(&ACK_CODE, 1);
      break;
    // Status request
    case 0xE9:
      packet[0] = ACK_CODE;
      packet_len = build_status_packet(&packet[1]);
      send_toHost(packet, packet_len + 1);
      break;
    // Read Data
    case 0xEB:
      packet[0] = ACK_CODE;
      packet_len = build_movement_packet(false, &packet[1]);
      send_toHost(packet, packet_len + 1);
      break;
    // Unknown command -> request resend
    default:
      send_toHost(&RESEND_CODE, 1);
      break;
  }
}

uint8_t PS2Mouse::build_status_packet(uint8_t* packet) {
  packet[0] = 0;
  packet[1] = resolution;
  packet[2] = sample_rate;

  if (!streaming_mode) packet[0] |= (1 << 6);
  if (data_reporting) packet[0] |= (1 << 5);
  if (scaling_2x1) packet[0] |= (1 << 4);
  if (state.buttons[0] > 0) packet[0] |= (1 << 2);
  if (state.buttons[2] > 0) packet[0] |= (1 << 1);
  if (state.buttons[1] > 0) packet[0] |= (1 << 0);

  return 3;  // return packet length
}

uint8_t PS2Mouse::build_movement_packet(boolean use_2x1_scaling, uint8_t* packet) {
  int16_t d_x = state.d_x;
  int16_t d_y = -state.d_y;          // y is inverted
  int16_t d_wheel = -state.d_wheel;  // wheel is inverted

  if (use_2x1_scaling) {
    d_x = apply_2x1_scaling(d_x);
    d_y = apply_2x1_scaling(d_y);
  }

  packet[0] = 1 << 3;  // bit3 is always 1
  packet[1] = (uint8_t)(d_x & 0xFF);
  packet[2] = (uint8_t)(d_y & 0xFF);

  if (abs(d_x) > UINT8_MAX) packet[0] |= (1 << 7);
  if (abs(d_y) > UINT8_MAX) packet[0] |= (1 << 6);
  if (d_y & 0x100) packet[0] |= (1 << 5);
  if (d_x & 0x100) packet[0] |= (1 << 4);
  if (state.buttons[2] > 0) packet[0] |= (1 << 2);
  if (state.buttons[1] > 0) packet[0] |= (1 << 1);
  if (state.buttons[0] > 0) packet[0] |= (1 << 0);

  // include scroll wheel state
  if (device_id >= DEVICE_ID_MOUSE_WHEEL) {
    if (d_wheel > 7) {
      d_wheel = 7;
    } else if (d_wheel < -8) {
      d_wheel = -8;
    }
    packet[3] = (uint8_t)(d_wheel & 0xFF);
  }

  // include button 4+5 state
  if (device_id >= DEVICE_ID_MOUSE_WHEEL_5BUTTONS) {
    packet[3] &= 0x0F;  // clear top 4 bits
    if (state.buttons[3] > 0) packet[3] |= (1 << 4);
    if (state.buttons[4] > 0) packet[3] |= (1 << 5);
  }

  return device_id >= DEVICE_ID_MOUSE_WHEEL ? 4 : 3;  // return packet length
}

int16_t PS2Mouse::apply_2x1_scaling(int16_t movement) {
  uint16_t abs_result = abs(movement);

  // scaling=2:1 table
  switch (abs_result) {
    case 0:
      abs_result = 0;
      break;
    case 1:
    case 2:
      abs_result = 1;
      break;
    case 3:
      abs_result = 3;
      break;
    case 4:
      abs_result = 6;
      break;
    case 5:
      abs_result = 9;
      break;
    default:
      abs_result <<= 1;
      break;
  }

  if (abs_result > UINT8_MAX) {
    abs_result = UINT8_MAX;
  }

  return (movement < 0) ? -abs_result : abs_result;
}
