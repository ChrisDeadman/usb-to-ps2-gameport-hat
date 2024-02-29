#include "SetupMode.h"

#include "Config.h"

#define PRESSED(key) (!(old_key_state & (key)) && (key_state & (key)))

static bool get_led_blink(unsigned long t_delta, uint8_t count);

SetupMode::SetupMode(VirtualKeyboard* const keyboard, JoystickState* const joystick_state)
    : keyboard(keyboard), joystick_state(joystick_state) {
  key_state = SetupKeyNone;
  item_idx = 0;
  in_setup_mode = false;
  in_edit_mode = false;
  swap_joy_axis_3_and_4 = false;
  emu_mode = EmuModeNone;
}

void SetupMode::task() {
  setup_mode_timer.tick();
  blink_timer.tick();

  // menu buttons
  SetupKeys old_key_state = key_state;
  key_state = get_key_state();

  // setup mode is triggered by holding down the setup key(s) for a duration
  if (!(key_state & SetupKeySetup)) {
    setup_mode_timer.reset();
  }
  if (setup_mode_timer.getElapsedMillis() > SETUP_ENTER_DELAY) {
    in_setup_mode = !in_setup_mode;
    in_edit_mode = false;
    key_state = SetupKeyNone;
    set_led_state(in_setup_mode, in_setup_mode);
    setup_mode_timer.reset();
    blink_timer.reset();
    return;
  }

  // do nothing if not in setup mode
  if (!in_setup_mode) {
    return;
  }

  if (set_item_value_quick(key_state)) {
    in_setup_mode = false;
    setup_mode_timer.reset();
    return;
  }

  // toggle edit mode
  if (PRESSED(SetupKeySelect)) {
    in_edit_mode = !in_edit_mode;
    return;
  }

  // left and right
  int8_t delta = 0;
  delta -= PRESSED(SetupKeyLeft) ? 1 : 0;
  delta += PRESSED(SetupKeyRight) ? 1 : 0;

  // switch through setup items
  if (!in_edit_mode) {
    int8_t new_idx = item_idx + delta;
    if (new_idx >= 0 && new_idx < item_count) {
      item_idx = (uint8_t)new_idx;
    }
  }

  // update item value
  if (in_edit_mode && delta != 0) {
    set_item_value(delta);
  }

  // A change resets blinking
  if (delta != 0) {
    blink_timer.reset();
  }

  // handle blink timer
  unsigned long blink_elapsed = blink_timer.getElapsedMillis();
  if (blink_elapsed > SETUP_BLINK_WINDOW) {
    blink_timer.reset();
  }

  // LED1 indicates menu item or edit mode
  bool led1 = get_led_blink(blink_elapsed, item_idx + 1) | in_edit_mode;

  // LED2 indicates menu item value
  uint8_t item_value = get_item_value();
  bool led2 = get_led_blink(blink_elapsed, item_value);

  // do not blink both leds simultaniously
  if (!in_edit_mode && (item_value > 0)) {
    led2 = true;
  }

  // do not blink too fast
  if (in_edit_mode && (item_value > 5)) {
    led2 = true;
  }

  // update led state
  set_led_state(led1, led2);
}

SetupKeys SetupMode::get_key_state() {
  SetupKeys key_state = SetupKeyNone;

  KeyboardModifierState kbd_modifiers = keyboard->pop_modifier_state();
  if ((kbd_modifiers & (ModLeftCtrl | ModRightCtrl)) &&
      (kbd_modifiers & (ModLeftShift | ModRightShift)) &&
      (kbd_modifiers & (ModLeftGUI | ModRightGUI))) {
    key_state = (SetupKeys)(key_state | SetupKeySetup);
  } else {
    keyboard->update_modifier_state(kbd_modifiers);
  }

  if (joystick_state->buttons[4] && joystick_state->buttons[5]) {
    key_state = (SetupKeys)(key_state | SetupKeySetup);
  }

  // don't consume key_state if not in setup mode
  if (!in_setup_mode) {
    return key_state;
  }

  KeyboardAction kb_action = keyboard->deq();
  if (kb_action.type == KbActionMake) {
    switch (kb_action.code) {
      case Return:
        key_state = (SetupKeys)(key_state | SetupKeySelect);
        break;
      case LeftArrow:
        key_state = (SetupKeys)(key_state | SetupKeyLeft);
        break;
      case RightArrow:
        key_state = (SetupKeys)(key_state | SetupKeyRight);
        break;
      case Escape:
        key_state = (SetupKeys)(key_state | SetupKeyQuick0);
        break;
      case F1:
        key_state = (SetupKeys)(key_state | SetupKeyQuick1);
        break;
      case F2:
        key_state = (SetupKeys)(key_state | SetupKeyQuick2);
        break;
      case F3:
        key_state = (SetupKeys)(key_state | SetupKeyQuick3);
        break;
      case F4:
        key_state = (SetupKeys)(key_state | SetupKeyQuick4);
        break;
      case F5:
        key_state = (SetupKeys)(key_state | SetupKeyQuick5);
        break;
      case F6:
        key_state = (SetupKeys)(key_state | SetupKeyQuick6);
        break;
      default:
        // put back what we don't consume
        keyboard->enq(kb_action);
        break;
    }
  } else {
    // put back what we don't consume
    keyboard->enq(kb_action);
  }

  if (joystick_state->buttons[0]) {
    key_state = (SetupKeys)(key_state | SetupKeySelect);
  }
  if (joystick_state->buttons[8]) {
    key_state = (SetupKeys)(key_state | SetupKeyQuick0);
  }
  if (JOY_AXIS_TEST(joystick_state->axes[0], false) ||
      JOY_AXIS_TEST(joystick_state->axes[4], false)) {
    key_state = (SetupKeys)(key_state | SetupKeyLeft);
  }
  if (JOY_AXIS_TEST(joystick_state->axes[0], true) ||
      JOY_AXIS_TEST(joystick_state->axes[4], true)) {
    key_state = (SetupKeys)(key_state | SetupKeyRight);
  }

  return key_state;
}

void SetupMode::set_led_state(bool led1, bool led2) {
  digitalWrite(EXT_LED1_PIN, led1 ? LOW : HIGH);
  digitalWrite(EXT_LED2_PIN, led2 ? LOW : HIGH);

  KeyboardLeds kbd_leds = KbLedNone;
  if (led1) kbd_leds = (KeyboardLeds)(kbd_leds | KbLedCapsLock);
  if (led2) kbd_leds = (KeyboardLeds)(kbd_leds | KbLedScrollLock | KbLedNumLock);
  keyboard->set_led_state(kbd_leds);
}

uint8_t SetupMode::get_item_value() {
  switch (item_idx) {
    case 0:
      return emu_mode;
    case 1:
      return swap_joy_axis_3_and_4 ? UINT8_MAX : 0;
    default:
      return 0;
  }
}

void SetupMode::set_item_value(int8_t delta) {
  int8_t new_value;

  switch (item_idx) {
    case 0:
      new_value = emu_mode + delta;
      if ((new_value >= (int8_t)EmuMode::EmuModeNone) &&
          (new_value <= (int8_t)EmuMode::EmuModeMouseJoy)) {
        emu_mode = (EmuMode)new_value;
      }
      break;
    case 1:
      swap_joy_axis_3_and_4 = !swap_joy_axis_3_and_4;
      break;
    default:
      break;
  }
}

bool SetupMode::set_item_value_quick(SetupKeys key_state) {
  switch (key_state) {
    case SetupKeyQuick0:
      emu_mode = EmuMode::EmuModeNone;
      swap_joy_axis_3_and_4 = false;
      return true;
    case SetupKeyQuick1:
      emu_mode = EmuMode::EmuModeJoyKeyb;
      return true;
    case SetupKeyQuick2:
      emu_mode = EmuMode::EmuModeJoyMouse;
      return true;
    case SetupKeyQuick3:
      emu_mode = EmuMode::EmuModeKeybJoy;
      return true;
    case SetupKeyQuick4:
      emu_mode = EmuMode::EmuModeKeybMouse;
      return true;
    case SetupKeyQuick5:
      emu_mode = EmuMode::EmuModeMouseJoy;
      return true;
    case SetupKeyQuick6:
      swap_joy_axis_3_and_4 = !swap_joy_axis_3_and_4;
      return true;
    default:
      return false;
  }
}

static bool get_led_blink(unsigned long t_delta, uint8_t count) {
  unsigned long window = SETUP_BLINK_WINDOW >> 1;
  if ((count < 1) || (t_delta > window)) {
    return false;
  }

  unsigned long blink_duration = (window << 1) / count;
  unsigned long blink_phase = (t_delta << 1) % blink_duration;
  return blink_phase > (blink_duration >> 1);
}
