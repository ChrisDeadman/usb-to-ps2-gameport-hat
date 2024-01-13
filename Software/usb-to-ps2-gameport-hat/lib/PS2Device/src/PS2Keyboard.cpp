#include "PS2Keyboard.h"

#define MAX_MAKE_PACKET_SIZE 8
#define MAX_BRK_PACKET_SIZE 3

struct MakeBreakCodeEntry {
  uint8_t make[MAX_MAKE_PACKET_SIZE];
  uint8_t brk[MAX_BRK_PACKET_SIZE];
};

// clang-format off
static const MakeBreakCodeEntry keycode_table[] = {
  {{0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xFF,0xFF,0xFF}}, // Overrun Error
  {{0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xFF,0xFF,0xFF}}, // POST Fail
  {{0x1C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x1C,0xFF}}, // a A
  {{0x32,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x32,0xFF}}, // b B
  {{0x21,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x21,0xFF}}, // c C
  {{0x23,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x23,0xFF}}, // d D
  {{0x24,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x24,0xFF}}, // e E
  {{0x2B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x2B,0xFF}}, // f F
  {{0x34,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x34,0xFF}}, // g G
  {{0x33,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x33,0xFF}}, // h H
  {{0x43,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x43,0xFF}}, // i I
  {{0x3B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x3B,0xFF}}, // j J
  {{0x42,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x42,0xFF}}, // k K
  {{0x4B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x4B,0xFF}}, // l L
  {{0x3A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x3A,0xFF}}, // m M
  {{0x31,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x31,0xFF}}, // n N
  {{0x44,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x44,0xFF}}, // o O
  {{0x4D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x4D,0xFF}}, // p P
  {{0x15,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x15,0xFF}}, // q Q
  {{0x2D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x2D,0xFF}}, // r R
  {{0x1B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x1B,0xFF}}, // s S
  {{0x2C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x2C,0xFF}}, // t T
  {{0x3C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x3C,0xFF}}, // u U
  {{0x2A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x2A,0xFF}}, // v V
  {{0x1D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x1D,0xFF}}, // w W
  {{0x22,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x22,0xFF}}, // x X
  {{0x35,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x35,0xFF}}, // y Y
  {{0x1A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x1A,0xFF}}, // z Z
  {{0x16,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x16,0xFF}}, // 1 !
  {{0x1E,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x1E,0xFF}}, // 2 @
  {{0x26,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x26,0xFF}}, // 3 #
  {{0x25,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x25,0xFF}}, // 4 $
  {{0x2E,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x2E,0xFF}}, // 5 %
  {{0x36,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x36,0xFF}}, // 6 ^
  {{0x3D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x3D,0xFF}}, // 7 &
  {{0x3E,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x3E,0xFF}}, // 8 *
  {{0x46,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x46,0xFF}}, // 9 (
  {{0x45,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x45,0xFF}}, // 0 )
  {{0x5A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x5A,0xFF}}, // Return
  {{0x76,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x76,0xFF}}, // Escape
  {{0x66,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x66,0xFF}}, // Backspace
  {{0x0D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x0D,0xFF}}, // Tab
  {{0x29,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x29,0xFF}}, // Space
  {{0x4E,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x4E,0xFF}}, // - _
  {{0x55,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x55,0xFF}}, // = +
  {{0x54,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x54,0xFF}}, // [ {
  {{0x5B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x5B,0xFF}}, // ] }
  {{0x5D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x5D,0xFF}}, // \ |
  {{0x5D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x5D,0xFF}}, // Europe 1
  {{0x4C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x4C,0xFF}}, // ; :
  {{0x52,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x52,0xFF}}, // ' "
  {{0x0E,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x0E,0xFF}}, // ` ~
  {{0x41,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x41,0xFF}}, // , <
  {{0x49,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x49,0xFF}}, // . >
  {{0x4A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x4A,0xFF}}, // / ?
  {{0x58,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x58,0xFF}}, // Caps Lock
  {{0x05,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x05,0xFF}}, // F1
  {{0x06,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x06,0xFF}}, // F2
  {{0x04,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x04,0xFF}}, // F3
  {{0x0C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x0C,0xFF}}, // F4
  {{0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x03,0xFF}}, // F5
  {{0x0B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x0B,0xFF}}, // F6
  {{0x83,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x83,0xFF}}, // F7
  {{0x0A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x0A,0xFF}}, // F8
  {{0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x01,0xFF}}, // F9
  {{0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x09,0xFF}}, // F10
  {{0x78,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x78,0xFF}}, // F11
  {{0x07,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x07,0xFF}}, // F12
  {{0xE0,0x7C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x7C}}, // Print Screen
  {{0x7E,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x7E,0xFF}}, // Scroll Lock
  {{0xE0,0xF0,0x7E,0xE0,0x7E,0xFF,0xFF,0xFF}, {0xFF,0xFF,0xFF}}, // Break (Ctrl-Pause)
  {{0xE1,0x14,0x77,0xE1,0xF0,0x14,0xF0,0x77}, {0xFF,0xFF,0xFF}}, // Pause
  {{0xE0,0x70,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x70}}, // Insert
  {{0xE0,0x6C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x6C}}, // Home
  {{0xE0,0x7D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x7D}}, // Page Up
  {{0xE0,0x71,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x71}}, // Delete
  {{0xE0,0x69,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x69}}, // End
  {{0xE0,0x7A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x7A}}, // Page Down
  {{0xE0,0x74,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x74}}, // Right Arrow
  {{0xE0,0x6B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x6B}}, // Left Arrow
  {{0xE0,0x72,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x72}}, // Down Arrow
  {{0xE0,0x75,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x75}}, // Up Arrow
  {{0x77,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x77,0xFF}}, // Num Lock
  {{0xE0,0x4A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x4A}}, // Keypad /
  {{0x7C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x7C,0xFF}}, // Keypad *
  {{0x7B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x7B,0xFF}}, // Keypad -
  {{0x79,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x79,0xFF}}, // Keypad +
  {{0xE0,0x5A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x5A}}, // Keypad Enter
  {{0x69,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x69,0xFF}}, // Keypad 1 End
  {{0x72,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x72,0xFF}}, // Keypad 2 Down
  {{0x7A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x7A,0xFF}}, // Keypad 3 PageDn
  {{0x6B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x6B,0xFF}}, // Keypad 4 Left
  {{0x73,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x73,0xFF}}, // Keypad 5
  {{0x74,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x74,0xFF}}, // Keypad 6 Right
  {{0x6C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x6C,0xFF}}, // Keypad 7 Home
  {{0x75,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x75,0xFF}}, // Keypad 8 Up
  {{0x7D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x7D,0xFF}}, // Keypad 9 PageUp
  {{0x70,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x70,0xFF}}, // Keypad 0 Insert
  {{0x71,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x71,0xFF}}, // Keypad . Delete
  {{0x61,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x61,0xFF}}, // Europe 2
  {{0xE0,0x2F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x2F}}, // App
  {{0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x0F,0xFF}}, // Keypad =
  {{0x2F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x2F,0xFF}}, // F13
  {{0x37,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x37,0xFF}}, // F14
  {{0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x3F,0xFF}}, // F15
  {{0x6D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x6D,0xFF}}, // Keypad , (Brazilian Keypad .)
  {{0x51,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x51,0xFF}}, // Keyboard Int'l 1 ろ (Ro)
  {{0x13,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x13,0xFF}}, // Keyboard Int'l 2 かたかな ひらがな ローマ字 (Katakana/Hiragana)
  {{0x6A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x6A,0xFF}}, // Keyboard Int'l 3 ￥ (Yen)
  {{0x64,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x64,0xFF}}, // Keyboard Int'l 4 前候補 変換 (次候補) 全候補 (Henkan)
  {{0x67,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x67,0xFF}}, // Keyboard Int'l 5 無変換 (Muhenkan)
  {{0x27,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x27,0xFF}}, // Keyboard Int'l 6 (PC9800 Keypad , )
  {{0xF2,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xFF,0xFF,0xFF}}, // Keyboard Lang 1 한/영 (Hanguel/English)
  {{0xF1,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xFF,0xFF,0xFF}}, // Keyboard Lang 2 한자 (Hanja)
  {{0x63,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x63,0xFF}}, // Keyboard Lang 3 かたかな (Katakana)
  {{0x62,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x62,0xFF}}, // Keyboard Lang 4 ひらがな (Hiragana)
  {{0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x5F,0xFF}}, // Keyboard Lang 5 半角/全角 (Zenkaku/Hankaku)
  {{0x14,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x14,0xFF}}, // Left Control
  {{0x12,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x12,0xFF}}, // Left Shift
  {{0x11,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x11,0xFF}}, // Left Alt
  {{0xE0,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x1F}}, // Left GUI
  {{0xE0,0x14,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x14}}, // Right Control
  {{0x59,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xF0,0x59,0xFF}}, // Right Shift
  {{0xE0,0x11,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x11}}, // Right Alt
  {{0xE0,0x27,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0xE0,0xF0,0x27}}, // Right GUI
};
// clang-format on

// milliseconds after first press
static const uint16_t typematic_delay_table[] = {250, 500, 750, 1000};

// milliseconds between characters
static const uint16_t typematic_rate_table[] = {
    33,  37,  42,  46,  48,  54,  58,  62,  67,  75,  83,
    92,  100, 109, 116, 125, 133, 149, 167, 182, 200, 217,
    233, 250, 270, 303, 333, 370, 400, 435, 476, 500};

static uint8_t get_keycode_entry_len(const uint8_t* entry, uint8_t size);
static bool requires_typematic_handling(KeyboardCodes key_code);

PS2Keyboard::PS2Keyboard(PS2Port* port) : PS2Device(port) { set_defaults(); }

void PS2Keyboard::set_defaults() {
  active_command = 0;
  last_keycode = KeyboardCodes::NoKey;

  enabled = true;
  typematic_delay = DEFAULT_TYPEMATIC_DELAY;
  typematic_rate = DEFAULT_TYPEMATIC_RATE;

  led_state = KeyboardLeds::LedAll;
}

KeyboardLeds PS2Keyboard::get_led_state() { return led_state; }

void PS2Keyboard::reset(bool send_ack) {
  set_defaults();
  uint8_t packet[3];
  uint8_t idx = 0;
  if (send_ack) {
    packet[idx++] = ACK_CODE;
  }
  packet[idx++] = BAT_OK;
  send_toHost(packet, idx);
}

void PS2Keyboard::enq_make(KeyboardCodes code) { make_buffer.enq(code); }

void PS2Keyboard::enq_brk(KeyboardCodes code) { brk_buffer.enq(code); }

void PS2Keyboard::task() {
  // wait until transmission is finished
  if (receiver.is_receiving() || sender.is_sending()) {
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

  // don't send if
  if (!enabled ||             // or keyboard is disabled
      (active_command != 0))  // or an active command is being processed
  {
    return;
  }

  // clock timers
  typematic_delay_timer.tick();
  typematic_rate_timer.tick();

  bool do_make = make_buffer.length() > 0;
  bool do_brk = brk_buffer.length() > 0;

  // handle make
  if (do_make) {
    // get keycode, update last keycode
    last_keycode = make_buffer.deq();

    // send make packet
    const uint8_t* packet = keycode_table[last_keycode].make;
    uint8_t packet_len = get_keycode_entry_len(packet, MAX_MAKE_PACKET_SIZE);
    if (packet_len > 0) {
      send_toHost(packet, packet_len);
    }
  }
  // handle break
  else if (do_brk) {
    // get keycode
    KeyboardCodes keycode = brk_buffer.deq();

    // clear last keycode
    if (keycode == last_keycode) {
      last_keycode = KeyboardCodes::NoKey;
    }

    // send break packet
    const uint8_t* packet = keycode_table[keycode].brk;
    uint8_t packet_len = get_keycode_entry_len(packet, MAX_BRK_PACKET_SIZE);
    if (packet_len > 0) {
      send_toHost(packet, packet_len);
    }
  }

  // typematic handling
  if (!do_make && requires_typematic_handling(last_keycode)) {
    // wait for typematic delay
    if (typematic_delay_timer.getElapsedMillis() <
        typematic_delay_table[typematic_delay]) {
      return;
    }
    // wait for typematic rate
    if (typematic_rate_timer.getElapsedMillis() <
        typematic_rate_table[typematic_rate]) {
      return;
    }

    // send make packet
    const uint8_t* packet = keycode_table[last_keycode].make;
    uint8_t packet_len = get_keycode_entry_len(packet, MAX_MAKE_PACKET_SIZE);
    if (packet_len > 0) {
      send_toHost(packet, packet_len);
    }

    // apply typematic rate again on next repeat
    typematic_rate_timer.reset();
  } else {
    // apply typematic delay after keycode change
    typematic_delay_timer.reset();
  }
}

void PS2Keyboard::send_toHost(const uint8_t* data, uint8_t len) {
  memcpy(send_buffer, data, len);
  send_buffer_idx = 0;
  send_buffer_len = len;
}

void PS2Keyboard::handle_active_command(uint8_t data_byte) {
  switch (active_command) {
    // Set Typematic Rate/Delay
    case (0xF3):
      typematic_rate = data_byte & 0x0F;
      typematic_delay = data_byte >> 4;
      active_command = 0;
      // No response
      break;
    // Set Scan Code Set
    case (0xF0):
      if (data_byte == 0x00) {
        // If the argument byte is 0x00, the keyboard responds with "ack",
        // followed by the current scan code set.
        uint8_t packet[2];
        packet[0] = ACK_CODE;
        packet[1] = SCAN_CODE_SET;
        send_toHost(packet, 2);
      } else {
        // TODO: Scancode set ignored, we always use Set 2
        send_toHost(&ACK_CODE, 1);
      }
      active_command = 0;
      break;
    // Set/Reset LEDs
    case (0xED):
      led_state = static_cast<KeyboardLeds>(data_byte);
      send_toHost(&ACK_CODE, 1);
      active_command = 0;
      break;
    // Unknown active command
    default:
      active_command = 0;
      break;
  }
}

void PS2Keyboard::handle_new_command(uint8_t data_byte) {
  // clear state after every command
  make_buffer.clear();
  brk_buffer.clear();

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
    // Disable
    case 0xF5:
      enabled = false;
      break;
    // Enable
    case 0xF4:
      enabled = true;
      break;
    // Set Typematic Rate/Delay
    case 0xF3:
      active_command = data_byte;
      send_toHost(&ACK_CODE, 1);
      break;
    // Get Device ID
    case 0xF2:
      send_toHost(DEVICE_ID, sizeof(DEVICE_ID));
      break;
    // Set Scancode-Set
    case 0xF0:
      active_command = data_byte;
      send_toHost(&ACK_CODE, 1);
      break;
    // Echo
    case 0xEE:
      uint8_t packet[1];
      packet[0] = 0xEE;
      send_toHost(packet, 1);
      break;
    // Set/Reset LEDs
    case 0xED:
      active_command = data_byte;
      send_toHost(&ACK_CODE, 1);
      break;
    // IGNORED
    case 0xFD:  // Set Key Type Make
    case 0xFC:  // Set Key Type Make/Break
    case 0xFB:  // Set Key Type Typematic
    case 0xFA:  // Set All Keys Typematic/Make/Break
    case 0xF9:  // Set All Keys Make
    case 0xF8:  // Set All Keys Make/Break
    case 0xF7:  // Set All Keys Typematic
      send_toHost(&ACK_CODE, 1);
      break;
    // Unknown command -> request resend
    default:
      send_toHost(&RESEND_CODE, 1);
      break;
  }
}

static uint8_t get_keycode_entry_len(const uint8_t* entry, uint8_t size) {
  uint8_t len;

  for (len = 0; len < size; ++len) {
    if (entry[len] == 0xFF) {
      break;
    }
  }

  return len;
}

static bool requires_typematic_handling(KeyboardCodes key_code) {
  if (key_code > KeyboardCodes::F15) {
    return false;
  }
  return get_keycode_entry_len(keycode_table[key_code].brk,
                               MAX_BRK_PACKET_SIZE) > 0;
}
