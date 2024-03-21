#ifndef _KEYBOARD_MODIFIER_STATE_H_
#define _KEYBOARD_MODIFIER_STATE_H_

enum KeyboardModifierState : uint8_t {
  ModNone = 0,
  ModLeftCtrl = 0x01,
  ModLeftShift = 0x02,
  ModLeftAlt = 0x04,
  ModLeftGUI = 0x08,
  ModRightCtrl = 0x10,
  ModRightShift = 0x20,
  ModRightAlt = 0x40,
  ModRightGUI = 0x80,
};

#endif  //_KEYBOARD_MODIFIER_STATE_H_
