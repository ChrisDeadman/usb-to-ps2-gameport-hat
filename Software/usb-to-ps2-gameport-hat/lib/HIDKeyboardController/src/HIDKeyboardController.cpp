#include "HIDKeyboardController.h"

#define KEY_START_INDEX (2)  // Starting index for key data in the HID buffer

static const KeyboardCodes keycode_table[] = {
    NoKey,
    OverrunError,
    POSTFail,
    NoKey,
    aA,
    bB,
    cC,
    dD,
    eE,
    fF,
    gG,
    hH,
    iI,
    jJ,
    kK,
    lL,
    mM,
    nN,
    oO,
    pP,
    qQ,
    rR,
    sS,
    tT,
    uU,
    vV,
    wW,
    xX,
    yY,
    zZ,
    Key1Exclamation,
    Key2At,
    Key3Hash,
    Key4Dollar,
    Key5Percent,
    Key6Caret,
    Key7Ampersand,
    Key8Asterisk,
    Key9LeftParen,
    Key0RightParen,
    Return,
    Escape,
    Backspace,
    Tab,
    Space,
    MinusUnderscore,
    EqualPlus,
    BracketLeftCurlyLeft,
    BracketRightCurlyRight,
    BackslashPipe,
    Europe1,
    SemicolonColon,
    QuoteDoubleQuote,
    BacktickTilde,
    CommaLess,
    PeriodGreater,
    SlashQuestion,
    CapsLock,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    PrintScreen,
    ScrollLock,
    Pause,  // Break if CTRL is presseed
    Insert,
    Home,
    PageUp,
    Delete,
    End,
    PageDown,
    RightArrow,
    LeftArrow,
    DownArrow,
    UpArrow,
    NumLock,
    KeypadDiv,
    KeypadMul,
    KeypadMinus,
    KeypadPlus,
    KeypadEnter,
    Keypad1End,
    Keypad2Down,
    Keypad3PageDn,
    Keypad4Left,
    Keypad5,
    Keypad6Right,
    Keypad7Home,
    Keypad8Up,
    Keypad9PageUp,
    Keypad0Insert,
    KeypadDotDelete,
    Europe2,
    App,
    NoKey,
    KeypadEqual,
    F13,
    F14,
    F15,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    Keypad,
    NoKey,
    KeyboardIntL1,
    KeyboardIntL2,
    KeyboardIntL3,
    KeyboardIntL4,
    KeyboardIntL5,
    KeyboardIntL6,
    NoKey,
    NoKey,
    NoKey,
    KeyboardLang1,
    KeyboardLang2,
    KeyboardLang3,
    KeyboardLang4,
    KeyboardLang5,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    NoKey,
    LeftControl,
    LeftShift,
    LeftAlt,
    LeftGUI,
    RightControl,
    RightShift,
    RightAlt,
    RightGUI,
};

extern "C" {
void __usb_kbd_dummy_callback(uint8_t const *const data, uint8_t length) {}
}
/**
 * implement in your code if you want to capture packages.
 */
void usb_data_received(uint8_t const *const data, uint8_t length)
    __attribute__((weak, alias("__usb_kbd_dummy_callback")));

void usb_data_sent(uint8_t const *const data, uint8_t length)
    __attribute__((weak, alias("__usb_kbd_dummy_callback")));

static KeyboardCodes hid_value_to_keyboard_code(uint8_t *buf,
                                                uint32_t len,
                                                uint32_t index,
                                                KeyboardModifierState modifier_state) {
  uint8_t hid_value = (len > (KEY_START_INDEX + index)) ? buf[KEY_START_INDEX + index] : 0;
  KeyboardCodes key = (hid_value < sizeof(keycode_table)) ? keycode_table[hid_value] : NoKey;

  // Handle Pause/Break key combination
  if (key == Pause && (modifier_state & (ModLeftCtrl | ModRightCtrl))) {
    key = Break;
  }

  return key;
}

HIDKeyboardController::HIDKeyboardController(HID *driver) : driver(driver) {
  driver->SetReportParser(0, this);
  modifier_state = ModNone;
  led_state = KbLedNone;
  memset(prev_state, NoKey, sizeof(prev_state));
}

bool HIDKeyboardController::is_connected() { return driver->isReady(); }

KeyboardModifierState HIDKeyboardController::get_modifier_state() { return modifier_state; };

KeyboardAction HIDKeyboardController::deq() {
  KeyboardAction kb_action;
  if (key_buffer.length() > 0) {
    kb_action = key_buffer.deq();
  } else {
    kb_action.type = KbActionNone;
  }
  return kb_action;
}

void HIDKeyboardController::set_led_state(KeyboardLeds new_state) {
  if (!is_connected() || new_state == led_state) {
    return;
  }
  led_state = new_state;

  send_report_buffer[0] = 0;
  if (led_state & KbLedNumLock) send_report_buffer[0] |= 0x1;
  if (led_state & KbLedCapsLock) send_report_buffer[0] |= 0x2;
  if (led_state & KbLedScrollLock) send_report_buffer[0] |= 0x4;

  driver->SetReport(0, 0, 2, 0, 1, send_report_buffer);
  usb_data_sent(send_report_buffer, 1);
}

void HIDKeyboardController::Parse(HID * /* hid */,
                                  uint32_t /* is_rpt_id */,
                                  uint32_t len,
                                  uint8_t *buf) {
  KeyboardAction kb_action;
  KeyboardCodes key_pressed;
  KeyboardCodes key_pressed_prev;
  KeyboardModifierState old_modifier_state = modifier_state;

  usb_data_received(buf, (uint8_t)len);

  // handle modifier keys
  if (len > 0) {
    modifier_state = static_cast<KeyboardModifierState>(buf[0]);

    if ((old_modifier_state & ModLeftCtrl) != (modifier_state & ModLeftCtrl)) {
      kb_action.code = LeftControl;
      kb_action.type = (modifier_state & ModLeftCtrl) ? KbActionMake : KbActionBreak;
      key_buffer.enq(kb_action);
    }
    if ((old_modifier_state & ModLeftShift) != (modifier_state & ModLeftShift)) {
      kb_action.code = LeftShift;
      kb_action.type = (modifier_state & ModLeftShift) ? KbActionMake : KbActionBreak;
      key_buffer.enq(kb_action);
    }
    if ((old_modifier_state & ModLeftAlt) != (modifier_state & ModLeftAlt)) {
      kb_action.code = LeftAlt;
      kb_action.type = (modifier_state & ModLeftAlt) ? KbActionMake : KbActionBreak;
      key_buffer.enq(kb_action);
    }
    if ((old_modifier_state & ModLeftGUI) != (modifier_state & ModLeftGUI)) {
      kb_action.code = LeftGUI;
      kb_action.type = (modifier_state & ModLeftGUI) ? KbActionMake : KbActionBreak;
      key_buffer.enq(kb_action);
    }
    if ((old_modifier_state & ModRightCtrl) != (modifier_state & ModRightCtrl)) {
      kb_action.code = RightControl;
      kb_action.type = (modifier_state & ModRightCtrl) ? KbActionMake : KbActionBreak;
      key_buffer.enq(kb_action);
    }
    if ((old_modifier_state & ModRightShift) != (modifier_state & ModRightShift)) {
      kb_action.code = RightShift;
      kb_action.type = (modifier_state & ModRightShift) ? KbActionMake : KbActionBreak;
      key_buffer.enq(kb_action);
    }
    if ((old_modifier_state & ModRightAlt) != (modifier_state & ModRightAlt)) {
      kb_action.code = RightAlt;
      kb_action.type = (modifier_state & ModRightAlt) ? KbActionMake : KbActionBreak;
      key_buffer.enq(kb_action);
    }
    if ((old_modifier_state & ModRightGUI) != (modifier_state & ModRightGUI)) {
      kb_action.code = RightGUI;
      kb_action.type = (modifier_state & ModRightGUI) ? KbActionMake : KbActionBreak;
      key_buffer.enq(kb_action);
    }
  }

  for (uint32_t idx = 0; idx < sizeof(prev_state); idx++) {
    // Detect if a key was released (BREAK)
    key_pressed_prev = prev_state[idx];
    if (key_pressed_prev != NoKey) {
      // Ignore if still pressed
      for (uint32_t idx_now = 0; idx_now < sizeof(prev_state); idx_now++) {
        key_pressed = hid_value_to_keyboard_code(buf, len, idx_now, modifier_state);
        if (key_pressed_prev == key_pressed) {
          key_pressed_prev = NoKey;
        }
      }

      // No BREAK code for "Break" key
      if (key_pressed_prev == Break) {
        key_pressed_prev = NoKey;
      }

      // Enqueue BREAK
      if (key_pressed_prev != NoKey) {
        kb_action.code = key_pressed_prev;
        kb_action.type = KbActionBreak;
        key_buffer.enq(kb_action);
      }
    }

    // Detect new key presses (MAKE actions)
    key_pressed = hid_value_to_keyboard_code(buf, len, idx, modifier_state);
    if (key_pressed != NoKey) {
      // Ignore if still pressed
      for (uint32_t idx_prev = 0; idx_prev < sizeof(prev_state); idx_prev++) {
        key_pressed_prev = prev_state[idx_prev];
        if (key_pressed == key_pressed_prev) {
          key_pressed = NoKey;
        }
      }

      // Enqueue MAKE
      if (key_pressed != NoKey) {
        kb_action.code = key_pressed;
        kb_action.type = KbActionMake;
        key_buffer.enq(kb_action);
      }
    }
  }

  // Update the previous state with the current buffer
  for (uint32_t idx = 0; idx < sizeof(prev_state); idx++) {
    prev_state[idx] = hid_value_to_keyboard_code(buf, len, idx, modifier_state);
  }
};
