#include "HIDKeyboardController.h"

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

HIDKeyboardController::HIDKeyboardController(HID *driver) : driver(driver) {
  driver->SetReportParser(0, this);
  modifier_state = ModNone;
  led_state = LedNone;
  memset(prev_state, NoKey, USB_KEYBOARD_KRO);
}

bool HIDKeyboardController::is_connected() { return driver->isReady(); }

KeyboardModifierState HIDKeyboardController::get_modifier_state() {
  return modifier_state;
};

KeyboardCodes HIDKeyboardController::deq_make() {
  if (make_buffer.length() <= 0) {
    return NoKey;
  }
  return make_buffer.deq();
}

KeyboardCodes HIDKeyboardController::deq_brk() {
  if (brk_buffer.length() <= 0) {
    return NoKey;
  }
  return brk_buffer.deq();
}

void HIDKeyboardController::set_led_state(KeyboardLeds state) {
  if (!is_connected() || state == led_state) {
    return;
  }
  led_state = state;

  uint8_t report_data = 0x0;
  if (led_state & LedNumLock) report_data |= 0x1;
  if (led_state & LedCapsLock) report_data |= 0x2;
  if (led_state & LedScrollLock) report_data |= 0x4;

  driver->SetReport(0, 0, 2, 0, 1, &report_data);
  usb_data_sent(&report_data, 1);
}

void HIDKeyboardController::Parse(HID * /* hid */, uint32_t /* is_rpt_id */,
                                  uint32_t len, uint8_t *buf) {
  usb_data_received(buf, (uint8_t)len);

  // handle modifier keys
  if (len > 0) {
    KeyboardModifierState old_modifier_state = modifier_state;
    modifier_state = static_cast<KeyboardModifierState>(buf[0]);

    if ((old_modifier_state & ModLeftCtrl) != (modifier_state & ModLeftCtrl)) {
      if (modifier_state & ModLeftCtrl) {
        make_buffer.enq(LeftControl);
      } else {
        brk_buffer.enq(LeftControl);
      }
    }
    if ((old_modifier_state & ModLeftShift) != (modifier_state & ModLeftShift)) {
      if (modifier_state & ModLeftShift) {
        make_buffer.enq(LeftShift);
      } else {
        brk_buffer.enq(LeftShift);
      }
    }
    if ((old_modifier_state & ModLeftAlt) != (modifier_state & ModLeftAlt)) {
      if (modifier_state & ModLeftAlt) {
        make_buffer.enq(LeftAlt);
      } else {
        brk_buffer.enq(LeftAlt);
      }
    }
    if ((old_modifier_state & ModLeftGUI) != (modifier_state & ModLeftGUI)) {
      if (modifier_state & ModLeftGUI) {
        make_buffer.enq(LeftGUI);
      } else {
        brk_buffer.enq(LeftGUI);
      }
    }
    if ((old_modifier_state & ModRightCtrl) != (modifier_state & ModRightCtrl)) {
      if (modifier_state & ModRightCtrl) {
        make_buffer.enq(RightControl);
      } else {
        brk_buffer.enq(RightControl);
      }
    }
    if ((old_modifier_state & ModRightShift) != (modifier_state & ModRightShift)) {
      if (modifier_state & ModRightShift) {
        make_buffer.enq(RightShift);
      } else {
        brk_buffer.enq(RightShift);
      }
    }
    if ((old_modifier_state & ModRightAlt) != (modifier_state & ModRightAlt)) {
      if (modifier_state & ModRightAlt) {
        make_buffer.enq(RightAlt);
      } else {
        brk_buffer.enq(RightAlt);
      }
    }
    if ((old_modifier_state & ModRightGUI) != (modifier_state & ModRightGUI)) {
      if (modifier_state & ModRightGUI) {
        make_buffer.enq(RightGUI);
      } else {
        brk_buffer.enq(RightGUI);
      }
    }
  }

  // handle keys
  for (uint32_t i = 0; i < USB_KEYBOARD_KRO; i++) {
    bool down = (len > 2 + i) && (buf[2 + i] > 0);
    bool up = prev_state[i] != NoKey;

    for (uint32_t j = 0; j < USB_KEYBOARD_KRO; j++) {
      if ((len > 2 + i) && (buf[2 + i] == prev_state[j])) down = false;
      if ((len > 2 + j) && (buf[2 + j] == prev_state[i])) up = false;
    }

    if (down) {
      KeyboardCodes key = keycode_table[buf[2 + i]];
      // handle Pause / Break
      if ((key == Pause) && (modifier_state | (ModLeftCtrl | ModRightCtrl))) {
        key = Break;
      }
      // enqueue MAKE
      if (key != NoKey) {
        make_buffer.enq(key);
      }
    }

    if (up) {
      KeyboardCodes key = keycode_table[prev_state[i]];
      // enqueue BREAK
      if (key != NoKey) {
        brk_buffer.enq(key);
      }
    }
  }

  // update prev. state
  for (uint32_t i = 0; i < USB_KEYBOARD_KRO; i++) {
    // ignore overrun
    if (len > 2 + i) {
      prev_state[i] = buf[2 + i];
    } else
      prev_state[i] = NoKey;
  }
};
