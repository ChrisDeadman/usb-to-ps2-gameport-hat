#ifndef _KEYBOARD_CODES_H_
#define _KEYBOARD_CODES_H_

enum KeyboardCodes : uint8_t {
  OverrunError = 0,
  POSTFail,
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
  Key1Exclamation,  // !
  Key2At,           // @
  Key3Hash,         // #
  Key4Dollar,       // $
  Key5Percent,      // %
  Key6Caret,        // ^
  Key7Ampersand,    // &
  Key8Asterisk,     // *
  Key9LeftParen,    // (
  Key0RightParen,   // )
  Return,
  Escape,
  Backspace,
  Tab,
  Space,
  MinusUnderscore,         // - and _
  EqualPlus,               // = and +
  BracketLeftCurlyLeft,    // [ and {
  BracketRightCurlyRight,  // ] and }
  BackslashPipe,           // \ and |
  Europe1,                 // specific key
  SemicolonColon,          // ; and :
  QuoteDoubleQuote,        // \ and "
  BacktickTilde,           // ` and ~
  CommaLess,               // , and <
  PeriodGreater,           // . and >
  SlashQuestion,           // / and ?
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
  Break,
  Pause,
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
  KeypadEqual,
  F13,
  F14,
  F15,
  Keypad,
  KeyboardIntL1,  // ろ(Ro)
  KeyboardIntL2,  // かたかなひらがなローマ字(Katakana/Hiragana)
  KeyboardIntL3,  // ￥(Yen)
  KeyboardIntL4,  // 前候補変換(次候補)全候補(Henkan)
  KeyboardIntL5,  // 無変換(Muhenkan)
  KeyboardIntL6,  // PC9800Keypad
  KeyboardLang1,  // 한/영(Hanguel/English)
  KeyboardLang2,  // 한자(Hanja)
  KeyboardLang3,  // かたかな(Katakana)
  KeyboardLang4,  // ひらがな(Hiragana)
  KeyboardLang5,  // 半角/全角(Zenkaku/Hankaku)
  LeftControl,
  LeftShift,
  LeftAlt,
  LeftGUI,
  RightControl,
  RightShift,
  RightAlt,
  RightGUI,
  NoKey = 0xFF
};

#endif  //_KEYBOARD_CODES_H_
