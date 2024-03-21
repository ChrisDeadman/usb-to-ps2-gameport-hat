#ifdef PIO_UNIT_TESTING
#include <stdint.h>
#include <string.h>
#else
#include <Arduino.h>
#endif

#include "HIDKeyboardParser.h"

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

static void insertion_sort(KeyboardCodes *keys, uint32_t size) {
  uint32_t i;
  uint32_t j;
  KeyboardCodes key;

  for (i = 1; i < size; ++i) {
    key = keys[i];
    j = i;
    while ((j > 0) && (keys[j - 1] > key)) {
      keys[j] = keys[j - 1];
      --j;
    }
    keys[j] = key;
  }
}

HIDKeyboardParser::HIDKeyboardParser() {
  modifier_state = ModNone;
  key_state_len = 0;
}

KeyboardModifierState HIDKeyboardParser::get_modifier_state() { return modifier_state; };

KeyboardAction HIDKeyboardParser::deq() {
  KeyboardAction kb_action;
  if (key_buffer.length() > 0) {
    kb_action = key_buffer.deq();
  } else {
    kb_action.type = KbActionNone;
  }
  return kb_action;
}

void HIDKeyboardParser::update(uint32_t len, uint8_t *buf) {
  uint32_t i;
  uint32_t j;
  uint8_t scancode;
  KeyboardAction kb_action;
  KeyboardModifierState old_modifier_state = modifier_state;
  KeyboardCodes prev_key;
  KeyboardCodes curr_key;
  KeyboardCodes prev_key_state[KEYBOARD_KRO];
  uint32_t prev_key_state_len;

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

  // Copy previous state
  if (key_state_len > 0) {
    memcpy(prev_key_state, key_state, key_state_len);
  }
  prev_key_state_len = key_state_len;

  // Update current state
  key_state_len = 0;
  for (i = 0; i < KEYBOARD_KRO; ++i) {
    if (len > (KEY_START_INDEX + i)) {
      // Get currently pressed key
      scancode = buf[KEY_START_INDEX + i];
      curr_key = (scancode < sizeof(keycode_table)) ? keycode_table[scancode] : NoKey;

      // Add pressed key if valid
      if ((curr_key != NoKey) && (curr_key != OverrunError)) {
        key_state[key_state_len++] = curr_key;
      }
    }
  }

  // Sort new key state
  insertion_sort(key_state, key_state_len);

  // Compare the sorted states
  i = 0;
  j = 0;
  while ((i < prev_key_state_len) || (j < key_state_len)) {
    prev_key = (i < prev_key_state_len) ? prev_key_state[i] : NoKey;
    curr_key = (j < key_state_len) ? key_state[j] : NoKey;

    if (prev_key == curr_key) {
      // Key is still pressed
      ++i;
      ++j;
    } else if (prev_key < curr_key) {
      // Key was released (BREAK)
      if ((prev_key != NoKey) && (prev_key != Break)) {
        kb_action.code = prev_key;
        kb_action.type = KbActionBreak;
        key_buffer.enq(kb_action);
      }
      ++i;
    } else {
      // New key pressed (MAKE)
      if (curr_key != NoKey) {
        kb_action.code = curr_key;
        kb_action.type = KbActionMake;
        key_buffer.enq(kb_action);
      }
      ++j;
    }
  }
};
