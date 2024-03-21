#ifndef _KEYBOARD_ACTION_H_
#define _KEYBOARD_ACTION_H_

#include "KeyboardCodes.h"

enum KeyboardActionTypes : uint8_t {
  KbActionNone = 0,
  KbActionMake = 1,
  KbActionBreak = 2,
};

struct KeyboardAction {
  KeyboardActionTypes type;
  KeyboardCodes code;
};

#endif  //_KEYBOARD_ACTION_H_
