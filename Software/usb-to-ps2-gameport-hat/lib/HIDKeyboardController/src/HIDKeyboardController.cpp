#include "HIDKeyboardController.h"

static const KeyboardCodes keycode_table[] = {
    KeyboardCodes::NoKey,
    KeyboardCodes::OverrunError,
    KeyboardCodes::POSTFail,
    KeyboardCodes::NoKey,
    KeyboardCodes::aA,
    KeyboardCodes::bB,
    KeyboardCodes::cC,
    KeyboardCodes::dD,
    KeyboardCodes::eE,
    KeyboardCodes::fF,
    KeyboardCodes::gG,
    KeyboardCodes::hH,
    KeyboardCodes::iI,
    KeyboardCodes::jJ,
    KeyboardCodes::kK,
    KeyboardCodes::lL,
    KeyboardCodes::mM,
    KeyboardCodes::nN,
    KeyboardCodes::oO,
    KeyboardCodes::pP,
    KeyboardCodes::qQ,
    KeyboardCodes::rR,
    KeyboardCodes::sS,
    KeyboardCodes::tT,
    KeyboardCodes::uU,
    KeyboardCodes::vV,
    KeyboardCodes::wW,
    KeyboardCodes::xX,
    KeyboardCodes::yY,
    KeyboardCodes::zZ,
    KeyboardCodes::Key1Exclamation,
    KeyboardCodes::Key2At,
    KeyboardCodes::Key3Hash,
    KeyboardCodes::Key4Dollar,
    KeyboardCodes::Key5Percent,
    KeyboardCodes::Key6Caret,
    KeyboardCodes::Key7Ampersand,
    KeyboardCodes::Key8Asterisk,
    KeyboardCodes::Key9LeftParen,
    KeyboardCodes::Key0RightParen,
    KeyboardCodes::Return,
    KeyboardCodes::Escape,
    KeyboardCodes::Backspace,
    KeyboardCodes::Tab,
    KeyboardCodes::Space,
    KeyboardCodes::MinusUnderscore,
    KeyboardCodes::EqualPlus,
    KeyboardCodes::BracketLeftCurlyLeft,
    KeyboardCodes::BracketRightCurlyRight,
    KeyboardCodes::BackslashPipe,
    KeyboardCodes::Europe1,
    KeyboardCodes::SemicolonColon,
    KeyboardCodes::QuoteDoubleQuote,
    KeyboardCodes::BacktickTilde,
    KeyboardCodes::CommaLess,
    KeyboardCodes::PeriodGreater,
    KeyboardCodes::SlashQuestion,
    KeyboardCodes::CapsLock,
    KeyboardCodes::F1,
    KeyboardCodes::F2,
    KeyboardCodes::F3,
    KeyboardCodes::F4,
    KeyboardCodes::F5,
    KeyboardCodes::F6,
    KeyboardCodes::F7,
    KeyboardCodes::F8,
    KeyboardCodes::F9,
    KeyboardCodes::F10,
    KeyboardCodes::F11,
    KeyboardCodes::F12,
    KeyboardCodes::PrintScreen,
    KeyboardCodes::ScrollLock,
    KeyboardCodes::Pause,  // KeyboardCodes::Break if CTRL is presseed
    KeyboardCodes::Insert,
    KeyboardCodes::Home,
    KeyboardCodes::PageUp,
    KeyboardCodes::Delete,
    KeyboardCodes::End,
    KeyboardCodes::PageDown,
    KeyboardCodes::RightArrow,
    KeyboardCodes::LeftArrow,
    KeyboardCodes::DownArrow,
    KeyboardCodes::UpArrow,
    KeyboardCodes::NumLock,
    KeyboardCodes::KeypadDiv,
    KeyboardCodes::KeypadMul,
    KeyboardCodes::KeypadMinus,
    KeyboardCodes::KeypadPlus,
    KeyboardCodes::KeypadEnter,
    KeyboardCodes::Keypad1End,
    KeyboardCodes::Keypad2Down,
    KeyboardCodes::Keypad3PageDn,
    KeyboardCodes::Keypad4Left,
    KeyboardCodes::Keypad5,
    KeyboardCodes::Keypad6Right,
    KeyboardCodes::Keypad7Home,
    KeyboardCodes::Keypad8Up,
    KeyboardCodes::Keypad9PageUp,
    KeyboardCodes::Keypad0Insert,
    KeyboardCodes::KeypadDotDelete,
    KeyboardCodes::Europe2,
    KeyboardCodes::App,
    KeyboardCodes::NoKey,
    KeyboardCodes::KeypadEqual,
    KeyboardCodes::F13,
    KeyboardCodes::F14,
    KeyboardCodes::F15,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::Keypad,
    KeyboardCodes::NoKey,
    KeyboardCodes::KeyboardIntL1,
    KeyboardCodes::KeyboardIntL2,
    KeyboardCodes::KeyboardIntL3,
    KeyboardCodes::KeyboardIntL4,
    KeyboardCodes::KeyboardIntL5,
    KeyboardCodes::KeyboardIntL6,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::KeyboardLang1,
    KeyboardCodes::KeyboardLang2,
    KeyboardCodes::KeyboardLang3,
    KeyboardCodes::KeyboardLang4,
    KeyboardCodes::KeyboardLang5,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::NoKey,
    KeyboardCodes::LeftControl,
    KeyboardCodes::LeftShift,
    KeyboardCodes::LeftAlt,
    KeyboardCodes::LeftGUI,
    KeyboardCodes::RightControl,
    KeyboardCodes::RightShift,
    KeyboardCodes::RightAlt,
    KeyboardCodes::RightGUI,
};

extern "C" {
void __usb_kbd_dummy_received_callback(uint8_t const *const data, uint8_t length) {}
}
/**
 * implement in your code if you want to capture packages.
 */
void usb_data_received(uint8_t const *const data, uint8_t length)
    __attribute__((weak, alias("__usb_kbd_dummy_received_callback")));

HIDKeyboardController::HIDKeyboardController(HID *driver) : driver(driver) {
  driver->SetReportParser(0, this);
  connected = false;
  modifier_state = ModifierState::ModNone;
  led_state = KeyboardLeds::LedNone;
  memset(prev_state, 0x00, USB_KEYBOARD_KRO);
}

bool HIDKeyboardController::is_connected() { return connected; }

ModifierState HIDKeyboardController::get_modifier_state() { return modifier_state; };

KeyboardCodes HIDKeyboardController::deq_make() {
  if (make_buffer.length() <= 0) {
    return KeyboardCodes::NoKey;
  }
  return make_buffer.deq();
}

KeyboardCodes HIDKeyboardController::deq_brk() {
  if (brk_buffer.length() <= 0) {
    return KeyboardCodes::NoKey;
  }
  return brk_buffer.deq();
}

void HIDKeyboardController::set_led_state(KeyboardLeds state) {
  if (state == led_state) {
    return;
  }
  led_state = state;

  uint8_t report_data = 0x0;
  if (led_state & KeyboardLeds::LedNumLock) report_data |= 0x1;
  if (led_state & KeyboardLeds::LedCapsLock) report_data |= 0x2;
  if (led_state & KeyboardLeds::LedScrollLock) report_data |= 0x4;

  driver->SetReport(0, 0, 2, 0, 1, &report_data);
}

void HIDKeyboardController::Parse(HID * /* hid */, uint32_t /* is_rpt_id */,
                                  uint32_t len, uint8_t *buf) {
  usb_data_received(buf, (uint8_t)len);
  connected = true;

  // handle modifier keys
  if (len > 0) {
    ModifierState old_modifier_state = modifier_state;
    modifier_state = static_cast<ModifierState>(buf[0]);

    if ((old_modifier_state & ModifierState::ModLeftCtrl) !=
        (modifier_state & ModifierState::ModLeftCtrl)) {
      if (modifier_state & ModifierState::ModLeftCtrl) {
        make_buffer.enq(KeyboardCodes::LeftControl);
      } else {
        brk_buffer.enq(KeyboardCodes::LeftControl);
      }
    }
    if ((old_modifier_state & ModifierState::ModLeftShift) !=
        (modifier_state & ModifierState::ModLeftShift)) {
      if (modifier_state & ModifierState::ModLeftShift) {
        make_buffer.enq(KeyboardCodes::LeftShift);
      } else {
        brk_buffer.enq(KeyboardCodes::LeftShift);
      }
    }
    if ((old_modifier_state & ModifierState::ModLeftAlt) !=
        (modifier_state & ModifierState::ModLeftAlt)) {
      if (modifier_state & ModifierState::ModLeftAlt) {
        make_buffer.enq(KeyboardCodes::LeftAlt);
      } else {
        brk_buffer.enq(KeyboardCodes::LeftAlt);
      }
    }
    if ((old_modifier_state & ModifierState::ModLeftGUI) !=
        (modifier_state & ModifierState::ModLeftGUI)) {
      if (modifier_state & ModifierState::ModLeftGUI) {
        make_buffer.enq(KeyboardCodes::LeftGUI);
      } else {
        brk_buffer.enq(KeyboardCodes::LeftGUI);
      }
    }
    if ((old_modifier_state & ModifierState::ModRightCtrl) !=
        (modifier_state & ModifierState::ModRightCtrl)) {
      if (modifier_state & ModifierState::ModRightCtrl) {
        make_buffer.enq(KeyboardCodes::RightControl);
      } else {
        brk_buffer.enq(KeyboardCodes::RightControl);
      }
    }
    if ((old_modifier_state & ModifierState::ModRightShift) !=
        (modifier_state & ModifierState::ModRightShift)) {
      if (modifier_state & ModifierState::ModRightShift) {
        make_buffer.enq(KeyboardCodes::RightShift);
      } else {
        brk_buffer.enq(KeyboardCodes::RightShift);
      }
    }
    if ((old_modifier_state & ModifierState::ModRightAlt) !=
        (modifier_state & ModifierState::ModRightAlt)) {
      if (modifier_state & ModifierState::ModRightAlt) {
        make_buffer.enq(KeyboardCodes::RightAlt);
      } else {
        brk_buffer.enq(KeyboardCodes::RightAlt);
      }
    }
    if ((old_modifier_state & ModifierState::ModRightGUI) !=
        (modifier_state & ModifierState::ModRightGUI)) {
      if (modifier_state & ModifierState::ModRightGUI) {
        make_buffer.enq(KeyboardCodes::RightGUI);
      } else {
        brk_buffer.enq(KeyboardCodes::RightGUI);
      }
    }
  }

  // handle keys
  for (uint32_t i = 0; i < USB_KEYBOARD_KRO; i++) {
    bool down = (len > 2 + i) && (buf[2 + i] != 1);
    bool up = prev_state[i] != 1;

    for (uint32_t j = 0; j < USB_KEYBOARD_KRO; j++) {
      if ((len > 2 + i) && (buf[2 + i] == prev_state[j])) down = false;
      if ((len > 2 + j) && (buf[2 + j] == prev_state[i])) up = false;
    }

    if (down) {
      KeyboardCodes key = keycode_table[buf[2 + i]];
      // handle Pause / Break
      if ((key == KeyboardCodes::Pause) &&
          (modifier_state |
           (ModifierState::ModLeftCtrl | ModifierState::ModRightCtrl))) {
        key = KeyboardCodes::Break;
      }
      // enqueue MAKE
      if (key != KeyboardCodes::NoKey) {
        make_buffer.enq(key);
      }
    }

    if (up) {
      KeyboardCodes key = keycode_table[prev_state[i]];
      // enqueue BREAK
      if (key != KeyboardCodes::NoKey) {
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
      prev_state[i] = 1;
  }
};
