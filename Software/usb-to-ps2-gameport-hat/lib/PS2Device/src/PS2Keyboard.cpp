#include "PS2Keyboard.h"

#define MAX_MAKE_PACKET_SIZE 8
#define MAX_BRK_PACKET_SIZE 3
#define NO_KEY KeyboardCodes::NoKey

struct MakeBreakCodeEntry {
  uint8_t make[MAX_MAKE_PACKET_SIZE];
  uint8_t brk[MAX_BRK_PACKET_SIZE];
};

// clang-format off
static const MakeBreakCodeEntry keycode_table[] = {
  {{0x00,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {NO_KEY,NO_KEY,NO_KEY}}, // Overrun Error
  {{0xFC,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {NO_KEY,NO_KEY,NO_KEY}}, // POST Fail
  {{0x1C,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x1C,NO_KEY}},     // a A
  {{0x32,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x32,NO_KEY}},     // b B
  {{0x21,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x21,NO_KEY}},     // c C
  {{0x23,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x23,NO_KEY}},     // d D
  {{0x24,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x24,NO_KEY}},     // e E
  {{0x2B,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x2B,NO_KEY}},     // f F
  {{0x34,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x34,NO_KEY}},     // g G
  {{0x33,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x33,NO_KEY}},     // h H
  {{0x43,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x43,NO_KEY}},     // i I
  {{0x3B,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x3B,NO_KEY}},     // j J
  {{0x42,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x42,NO_KEY}},     // k K
  {{0x4B,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x4B,NO_KEY}},     // l L
  {{0x3A,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x3A,NO_KEY}},     // m M
  {{0x31,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x31,NO_KEY}},     // n N
  {{0x44,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x44,NO_KEY}},     // o O
  {{0x4D,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x4D,NO_KEY}},     // p P
  {{0x15,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x15,NO_KEY}},     // q Q
  {{0x2D,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x2D,NO_KEY}},     // r R
  {{0x1B,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x1B,NO_KEY}},     // s S
  {{0x2C,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x2C,NO_KEY}},     // t T
  {{0x3C,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x3C,NO_KEY}},     // u U
  {{0x2A,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x2A,NO_KEY}},     // v V
  {{0x1D,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x1D,NO_KEY}},     // w W
  {{0x22,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x22,NO_KEY}},     // x X
  {{0x35,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x35,NO_KEY}},     // y Y
  {{0x1A,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x1A,NO_KEY}},     // z Z
  {{0x16,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x16,NO_KEY}},     // 1 !
  {{0x1E,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x1E,NO_KEY}},     // 2 @
  {{0x26,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x26,NO_KEY}},     // 3 #
  {{0x25,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x25,NO_KEY}},     // 4 $
  {{0x2E,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x2E,NO_KEY}},     // 5 %
  {{0x36,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x36,NO_KEY}},     // 6 ^
  {{0x3D,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x3D,NO_KEY}},     // 7 &
  {{0x3E,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x3E,NO_KEY}},     // 8 *
  {{0x46,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x46,NO_KEY}},     // 9 (
  {{0x45,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x45,NO_KEY}},     // 0 )
  {{0x5A,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x5A,NO_KEY}},     // Return
  {{0x76,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x76,NO_KEY}},     // Escape
  {{0x66,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x66,NO_KEY}},     // Backspace
  {{0x0D,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x0D,NO_KEY}},     // Tab
  {{0x29,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x29,NO_KEY}},     // Space
  {{0x4E,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x4E,NO_KEY}},     // - _
  {{0x55,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x55,NO_KEY}},     // = +
  {{0x54,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x54,NO_KEY}},     // [ {
  {{0x5B,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x5B,NO_KEY}},     // ] }
  {{0x5D,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x5D,NO_KEY}},     // \ |
  {{0x5D,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x5D,NO_KEY}},     // Europe 1
  {{0x4C,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x4C,NO_KEY}},     // ; :
  {{0x52,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x52,NO_KEY}},     // ' "
  {{0x0E,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x0E,NO_KEY}},     // ` ~
  {{0x41,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x41,NO_KEY}},     // , <
  {{0x49,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x49,NO_KEY}},     // . >
  {{0x4A,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x4A,NO_KEY}},     // / ?
  {{0x58,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x58,NO_KEY}},     // Caps Lock
  {{0x05,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x05,NO_KEY}},     // F1
  {{0x06,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x06,NO_KEY}},     // F2
  {{0x04,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x04,NO_KEY}},     // F3
  {{0x0C,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x0C,NO_KEY}},     // F4
  {{0x03,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x03,NO_KEY}},     // F5
  {{0x0B,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x0B,NO_KEY}},     // F6
  {{0x83,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x83,NO_KEY}},     // F7
  {{0x0A,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x0A,NO_KEY}},     // F8
  {{0x01,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x01,NO_KEY}},     // F9
  {{0x09,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x09,NO_KEY}},     // F10
  {{0x78,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x78,NO_KEY}},     // F11
  {{0x07,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x07,NO_KEY}},     // F12
  {{0xE0,0x7C,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x7C}},       // Print Screen
  {{0x7E,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x7E,NO_KEY}},     // Scroll Lock
  {{0xE0,0xF0,0x7E,0xE0,0x7E,NO_KEY,NO_KEY,NO_KEY},         {NO_KEY,NO_KEY,NO_KEY}}, // Break (Ctrl-Pause)
  {{0xE1,0x14,0x77,0xE1,0xF0,0x14,0xF0,0x77},               {NO_KEY,NO_KEY,NO_KEY}}, // Pause
  {{0xE0,0x70,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x70}},       // Insert
  {{0xE0,0x6C,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x6C}},       // Home
  {{0xE0,0x7D,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x7D}},       // Page Up
  {{0xE0,0x71,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x71}},       // Delete
  {{0xE0,0x69,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x69}},       // End
  {{0xE0,0x7A,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x7A}},       // Page Down
  {{0xE0,0x74,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x74}},       // Right Arrow
  {{0xE0,0x6B,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x6B}},       // Left Arrow
  {{0xE0,0x72,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x72}},       // Down Arrow
  {{0xE0,0x75,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x75}},       // Up Arrow
  {{0x77,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x77,NO_KEY}},     // Num Lock
  {{0xE0,0x4A,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x4A}},       // Keypad /
  {{0x7C,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x7C,NO_KEY}},     // Keypad *
  {{0x7B,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x7B,NO_KEY}},     // Keypad -
  {{0x79,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x79,NO_KEY}},     // Keypad +
  {{0xE0,0x5A,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x5A}},       // Keypad Enter
  {{0x69,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x69,NO_KEY}},     // Keypad 1 End
  {{0x72,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x72,NO_KEY}},     // Keypad 2 Down
  {{0x7A,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x7A,NO_KEY}},     // Keypad 3 PageDn
  {{0x6B,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x6B,NO_KEY}},     // Keypad 4 Left
  {{0x73,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x73,NO_KEY}},     // Keypad 5
  {{0x74,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x74,NO_KEY}},     // Keypad 6 Right
  {{0x6C,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x6C,NO_KEY}},     // Keypad 7 Home
  {{0x75,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x75,NO_KEY}},     // Keypad 8 Up
  {{0x7D,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x7D,NO_KEY}},     // Keypad 9 PageUp
  {{0x70,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x70,NO_KEY}},     // Keypad 0 Insert
  {{0x71,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x71,NO_KEY}},     // Keypad . Delete
  {{0x61,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x61,NO_KEY}},     // Europe 2
  {{0xE0,0x2F,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x2F}},       // App
  {{0x0F,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x0F,NO_KEY}},     // Keypad =
  {{0x2F,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x2F,NO_KEY}},     // F13
  {{0x37,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x37,NO_KEY}},     // F14
  {{0x3F,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x3F,NO_KEY}},     // F15
  {{0x6D,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x6D,NO_KEY}},     // Keypad , (Brazilian Keypad .)
  {{0x51,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x51,NO_KEY}},     // Keyboard Int'l 1 ろ (Ro)
  {{0x13,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x13,NO_KEY}},     // Keyboard Int'l 2 かたかな ひらがな ローマ字 (Katakana/Hiragana)
  {{0x6A,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x6A,NO_KEY}},     // Keyboard Int'l 3 ￥ (Yen)
  {{0x64,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x64,NO_KEY}},     // Keyboard Int'l 4 前候補 変換 (次候補) 全候補 (Henkan)
  {{0x67,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x67,NO_KEY}},     // Keyboard Int'l 5 無変換 (Muhenkan)
  {{0x27,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x27,NO_KEY}},     // Keyboard Int'l 6 (PC9800 Keypad , )
  {{0xF2,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {NO_KEY,NO_KEY,NO_KEY}}, // Keyboard Lang 1 한/영 (Hanguel/English)
  {{0xF1,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {NO_KEY,NO_KEY,NO_KEY}}, // Keyboard Lang 2 한자 (Hanja)
  {{0x63,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x63,NO_KEY}},     // Keyboard Lang 3 かたかな (Katakana)
  {{0x62,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x62,NO_KEY}},     // Keyboard Lang 4 ひらがな (Hiragana)
  {{0x5F,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x5F,NO_KEY}},     // Keyboard Lang 5 半角/全角 (Zenkaku/Hankaku)
  {{0x14,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x14,NO_KEY}},     // Left Control
  {{0x12,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x12,NO_KEY}},     // Left Shift
  {{0x11,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x11,NO_KEY}},     // Left Alt
  {{0xE0,0x1F,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x1F}},       // Left GUI
  {{0xE0,0x14,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x14}},       // Right Control
  {{0x59,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY}, {0xF0,0x59,NO_KEY}},     // Right Shift
  {{0xE0,0x11,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x11}},       // Right Alt
  {{0xE0,0x27,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY},   {0xE0,0xF0,0x27}},       // Right GUI
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
    if (entry[len] == NO_KEY) {
      break;
    }
  }

  return len;
}

static bool requires_typematic_handling(KeyboardCodes key_code) {
  if (key_code > KeyboardCodes::F15) {
    return false;
  }
  return get_keycode_entry_len(keycode_table[key_code].brk, MAX_BRK_PACKET_SIZE) > 0;
}
