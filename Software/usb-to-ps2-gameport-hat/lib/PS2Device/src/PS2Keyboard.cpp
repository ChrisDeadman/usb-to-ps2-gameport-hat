#include "PS2Keyboard.h"

#define MAX_MAKE_PACKET_SIZE 8
#define MAX_BRK_PACKET_SIZE 3

struct KeycodeTableEntry {
  uint8_t make[MAX_MAKE_PACKET_SIZE];
  uint8_t brk[MAX_BRK_PACKET_SIZE];
};

// clang-format off
static const KeycodeTableEntry keycode_table[] = {
  {{0x00,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {NoKey,NoKey,NoKey}}, // Overrun Error
  {{0xFC,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {NoKey,NoKey,NoKey}}, // POST Fail
  {{0x1C,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x1C,NoKey}},     // a A
  {{0x32,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x32,NoKey}},     // b B
  {{0x21,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x21,NoKey}},     // c C
  {{0x23,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x23,NoKey}},     // d D
  {{0x24,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x24,NoKey}},     // e E
  {{0x2B,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x2B,NoKey}},     // f F
  {{0x34,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x34,NoKey}},     // g G
  {{0x33,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x33,NoKey}},     // h H
  {{0x43,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x43,NoKey}},     // i I
  {{0x3B,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x3B,NoKey}},     // j J
  {{0x42,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x42,NoKey}},     // k K
  {{0x4B,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x4B,NoKey}},     // l L
  {{0x3A,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x3A,NoKey}},     // m M
  {{0x31,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x31,NoKey}},     // n N
  {{0x44,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x44,NoKey}},     // o O
  {{0x4D,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x4D,NoKey}},     // p P
  {{0x15,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x15,NoKey}},     // q Q
  {{0x2D,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x2D,NoKey}},     // r R
  {{0x1B,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x1B,NoKey}},     // s S
  {{0x2C,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x2C,NoKey}},     // t T
  {{0x3C,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x3C,NoKey}},     // u U
  {{0x2A,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x2A,NoKey}},     // v V
  {{0x1D,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x1D,NoKey}},     // w W
  {{0x22,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x22,NoKey}},     // x X
  {{0x35,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x35,NoKey}},     // y Y
  {{0x1A,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x1A,NoKey}},     // z Z
  {{0x16,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x16,NoKey}},     // 1 !
  {{0x1E,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x1E,NoKey}},     // 2 @
  {{0x26,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x26,NoKey}},     // 3 #
  {{0x25,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x25,NoKey}},     // 4 $
  {{0x2E,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x2E,NoKey}},     // 5 %
  {{0x36,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x36,NoKey}},     // 6 ^
  {{0x3D,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x3D,NoKey}},     // 7 &
  {{0x3E,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x3E,NoKey}},     // 8 *
  {{0x46,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x46,NoKey}},     // 9 (
  {{0x45,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x45,NoKey}},     // 0 )
  {{0x5A,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x5A,NoKey}},     // Return
  {{0x76,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x76,NoKey}},     // Escape
  {{0x66,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x66,NoKey}},     // Backspace
  {{0x0D,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x0D,NoKey}},     // Tab
  {{0x29,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x29,NoKey}},     // Space
  {{0x4E,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x4E,NoKey}},     // - _
  {{0x55,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x55,NoKey}},     // = +
  {{0x54,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x54,NoKey}},     // [ {
  {{0x5B,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x5B,NoKey}},     // ] }
  {{0x5D,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x5D,NoKey}},     // \ |
  {{0x5D,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x5D,NoKey}},     // Europe 1
  {{0x4C,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x4C,NoKey}},     // ; :
  {{0x52,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x52,NoKey}},     // ' "
  {{0x0E,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x0E,NoKey}},     // ` ~
  {{0x41,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x41,NoKey}},     // , <
  {{0x49,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x49,NoKey}},     // . >
  {{0x4A,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x4A,NoKey}},     // / ?
  {{0x58,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x58,NoKey}},     // Caps Lock
  {{0x05,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x05,NoKey}},     // F1
  {{0x06,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x06,NoKey}},     // F2
  {{0x04,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x04,NoKey}},     // F3
  {{0x0C,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x0C,NoKey}},     // F4
  {{0x03,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x03,NoKey}},     // F5
  {{0x0B,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x0B,NoKey}},     // F6
  {{0x83,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x83,NoKey}},     // F7
  {{0x0A,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x0A,NoKey}},     // F8
  {{0x01,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x01,NoKey}},     // F9
  {{0x09,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x09,NoKey}},     // F10
  {{0x78,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x78,NoKey}},     // F11
  {{0x07,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x07,NoKey}},     // F12
  {{0xE0,0x7C,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x7C}},       // Print Screen
  {{0x7E,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x7E,NoKey}},     // Scroll Lock
  {{0xE0,0xF0,0x7E,0xE0,0x7E,NoKey,NoKey,NoKey},         {NoKey,NoKey,NoKey}}, // Break (Ctrl-Pause)
  {{0xE1,0x14,0x77,0xE1,0xF0,0x14,0xF0,0x77},               {NoKey,NoKey,NoKey}}, // Pause
  {{0xE0,0x70,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x70}},       // Insert
  {{0xE0,0x6C,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x6C}},       // Home
  {{0xE0,0x7D,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x7D}},       // Page Up
  {{0xE0,0x71,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x71}},       // Delete
  {{0xE0,0x69,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x69}},       // End
  {{0xE0,0x7A,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x7A}},       // Page Down
  {{0xE0,0x74,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x74}},       // Right Arrow
  {{0xE0,0x6B,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x6B}},       // Left Arrow
  {{0xE0,0x72,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x72}},       // Down Arrow
  {{0xE0,0x75,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x75}},       // Up Arrow
  {{0x77,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x77,NoKey}},     // Num Lock
  {{0xE0,0x4A,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x4A}},       // Keypad /
  {{0x7C,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x7C,NoKey}},     // Keypad *
  {{0x7B,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x7B,NoKey}},     // Keypad -
  {{0x79,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x79,NoKey}},     // Keypad +
  {{0xE0,0x5A,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x5A}},       // Keypad Enter
  {{0x69,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x69,NoKey}},     // Keypad 1 End
  {{0x72,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x72,NoKey}},     // Keypad 2 Down
  {{0x7A,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x7A,NoKey}},     // Keypad 3 PageDn
  {{0x6B,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x6B,NoKey}},     // Keypad 4 Left
  {{0x73,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x73,NoKey}},     // Keypad 5
  {{0x74,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x74,NoKey}},     // Keypad 6 Right
  {{0x6C,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x6C,NoKey}},     // Keypad 7 Home
  {{0x75,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x75,NoKey}},     // Keypad 8 Up
  {{0x7D,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x7D,NoKey}},     // Keypad 9 PageUp
  {{0x70,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x70,NoKey}},     // Keypad 0 Insert
  {{0x71,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x71,NoKey}},     // Keypad . Delete
  {{0x61,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x61,NoKey}},     // Europe 2
  {{0xE0,0x2F,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x2F}},       // App
  {{0x0F,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x0F,NoKey}},     // Keypad =
  {{0x2F,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x2F,NoKey}},     // F13
  {{0x37,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x37,NoKey}},     // F14
  {{0x3F,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x3F,NoKey}},     // F15
  {{0x6D,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x6D,NoKey}},     // Keypad , (Brazilian Keypad .)
  {{0x51,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x51,NoKey}},     // Keyboard Int'l 1 ろ (Ro)
  {{0x13,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x13,NoKey}},     // Keyboard Int'l 2 かたかな ひらがな ローマ字 (Katakana/Hiragana)
  {{0x6A,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x6A,NoKey}},     // Keyboard Int'l 3 ￥ (Yen)
  {{0x64,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x64,NoKey}},     // Keyboard Int'l 4 前候補 変換 (次候補) 全候補 (Henkan)
  {{0x67,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x67,NoKey}},     // Keyboard Int'l 5 無変換 (Muhenkan)
  {{0x27,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x27,NoKey}},     // Keyboard Int'l 6 (PC9800 Keypad , )
  {{0xF2,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {NoKey,NoKey,NoKey}}, // Keyboard Lang 1 한/영 (Hanguel/English)
  {{0xF1,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {NoKey,NoKey,NoKey}}, // Keyboard Lang 2 한자 (Hanja)
  {{0x63,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x63,NoKey}},     // Keyboard Lang 3 かたかな (Katakana)
  {{0x62,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x62,NoKey}},     // Keyboard Lang 4 ひらがな (Hiragana)
  {{0x5F,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x5F,NoKey}},     // Keyboard Lang 5 半角/全角 (Zenkaku/Hankaku)
  {{0x14,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x14,NoKey}},     // Left Control
  {{0x12,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x12,NoKey}},     // Left Shift
  {{0x11,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x11,NoKey}},     // Left Alt
  {{0xE0,0x1F,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x1F}},       // Left GUI
  {{0xE0,0x14,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x14}},       // Right Control
  {{0x59,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey}, {0xF0,0x59,NoKey}},     // Right Shift
  {{0xE0,0x11,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x11}},       // Right Alt
  {{0xE0,0x27,NoKey,NoKey,NoKey,NoKey,NoKey,NoKey},   {0xE0,0xF0,0x27}},       // Right GUI
};
// clang-format on

// milliseconds after first press
static const uint16_t typematic_delay_table[] = {250, 500, 750, 1000};

// milliseconds between characters
static const uint16_t typematic_rate_table[] = {
    33,  37,  42,  46,  48,  54,  58,  62,  67,  75,  83,  92,  100, 109, 116, 125,
    133, 149, 167, 182, 200, 217, 233, 250, 270, 303, 333, 370, 400, 435, 476, 500};

static uint8_t get_keycode_entry_len(const uint8_t* entry, uint8_t size);
static bool requires_typematic_handling(KeyboardCodes key_code);

PS2Keyboard::PS2Keyboard(PS2Port* port) : PS2Device(port) {
  send_buffer_idx = 0;
  send_buffer_len = 0;
  set_defaults();
}

void PS2Keyboard::set_defaults() {
  active_command = 0;
  last_action.type = KbActionNone;

  enabled = true;
  typematic_delay = DEFAULT_TYPEMATIC_DELAY;
  typematic_rate = DEFAULT_TYPEMATIC_RATE;

  led_state = KbLedAll;
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

void PS2Keyboard::enq(KeyboardAction kb_action) {
  if (kb_action.type != KbActionNone) {
    key_buffer.enq(kb_action);
  }
}

void PS2Keyboard::task() {
  // wait until transmission is finished
  if (receiver.is_busy() || sender.is_busy()) {
    typematic_rate_timer.reset();
    return;
  }

  // data received
  if (receiver.has_data()) {
    uint8_t data_byte = receiver.pop_data();
    bool data_valid = receiver.is_data_valid();
    ps2_data_received(port->data_pin, data_byte, data_valid);  // debug logging
    // stop sending in case we currently are
    send_buffer_idx = send_buffer_len;
    // invoke command handlers
    if (data_valid) {
      if (active_command) {
        handle_active_command(data_byte);
      } else {
        handle_new_command(data_byte);
      }
    }
    return;
  }

  // check if we have anything to send
  if (send_buffer_idx < send_buffer_len) {
    uint8_t data_byte = send_buffer[send_buffer_idx++];
    ps2_data_sent(port->data_pin, data_byte);  // debug logging
    sender.begin_send(data_byte);
    return;
  }

  // don't send if
  if (!enabled ||             // keyboard is disabled
      (active_command != 0))  // or an active command is being processed
  {
    return;
  }

  // clock timers
  typematic_delay_timer.tick();
  typematic_rate_timer.tick();

  // get next action
  KeyboardAction kb_action;
  if (key_buffer.length() > 0) {
    kb_action = key_buffer.deq();
  } else {
    kb_action.type = KbActionNone;
  }

  // handle action
  if (kb_action.type != KbActionNone) {
    const uint8_t* packet;
    uint8_t packet_len;

    // ignore if same as last action
    if (kb_action.type == last_action.type && kb_action.code == last_action.code) {
      return;
    }

    // save last action
    last_action = kb_action;

    // build MAKE packet
    if (kb_action.type == KbActionMake) {
      packet = keycode_table[kb_action.code].make;
      packet_len = get_keycode_entry_len(packet, MAX_MAKE_PACKET_SIZE);
    }
    // build BREAK packet
    else {
      packet = keycode_table[kb_action.code].brk;
      packet_len = get_keycode_entry_len(packet, MAX_BRK_PACKET_SIZE);
    }

    // send packet
    if (packet_len > 0) {
      send_toHost(packet, packet_len);
    }

    // apply typematic delay after keycode change
    typematic_delay_timer.reset();
  }

  // typematic handling
  if (last_action.type == KbActionMake &&
      requires_typematic_handling(last_action.code)) {
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
    const uint8_t* packet = keycode_table[last_action.code].make;
    uint8_t packet_len = get_keycode_entry_len(packet, MAX_MAKE_PACKET_SIZE);
    if (packet_len > 0) {
      send_toHost(packet, packet_len);
    }

    // apply typematic rate again on next repeat
    typematic_rate_timer.reset();
  }
}

void PS2Keyboard::resend() { send_buffer_idx = 0; }

void PS2Keyboard::send_toHost(const uint8_t* data, uint8_t length) {
  memcpy(send_buffer, data, length);
  send_buffer_idx = 0;
  send_buffer_len = length;
}

void PS2Keyboard::handle_active_command(uint8_t data_byte) {
  switch (active_command) {
    // Set Typematic Rate/Delay
    case (0xF3):
      typematic_rate = data_byte & 0x0F;
      typematic_delay = data_byte >> 4;
      active_command = 0;
      send_toHost(&ACK_CODE, 1);
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
  key_buffer.clear();

  switch (data_byte) {
    // Reset
    case 0xFF:
      reset(true);
      break;
    // Resend
    case 0xFE:
      resend();
      break;
    // Set defaults
    case 0xF6:
      set_defaults();
      send_toHost(&ACK_CODE, 1);
      break;
    // Disable
    case 0xF5:
      enabled = false;
      send_toHost(&ACK_CODE, 1);
      break;
    // Enable
    case 0xF4:
      enabled = true;
      send_toHost(&ACK_CODE, 1);
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
    if (entry[len] == NoKey) {
      break;
    }
  }

  return len;
}

static bool requires_typematic_handling(KeyboardCodes key_code) {
  if (key_code > F15) {
    return false;
  }
  return get_keycode_entry_len(keycode_table[key_code].brk, MAX_BRK_PACKET_SIZE) > 0;
}
