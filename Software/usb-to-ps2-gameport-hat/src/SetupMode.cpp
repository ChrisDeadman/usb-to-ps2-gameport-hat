#include "SetupMode.h"

#include "Config.h"

#define PRESSED(key) (!(old_key_state & (key)) && (key_state & (key)))

static bool get_led_blink(unsigned long t_delta, uint8_t count);

SetupMode::SetupMode(VirtualKeyboard* const keyboard,
                     JoystickState* const joystick_state)
    : keyboard(keyboard), joystick_state(joystick_state) {
  key_state = SetupKeys::None;
  item_idx = 0;
  in_setup_mode = false;
  in_edit_mode = false;
  swap_joy_axis_3_and_4 = true;
  dummy_value = 0;
}

void SetupMode::task() {
  setup_mode_timer.tick();
  blink_timer.tick();

  // menu buttons
  SetupKeys old_key_state = key_state;
  key_state = get_key_state();

  // setup mode is triggered by holding down the setup key(s) for a duration
  if (!(key_state & SetupKeys::Setup)) {
    setup_mode_timer.reset();
  }
  if (setup_mode_timer.getElapsedMillis() > SETUP_ENTER_DELAY) {
    in_setup_mode = !in_setup_mode;
    in_edit_mode = false;
    key_state = SetupKeys::None;
    set_led_state(in_setup_mode, in_setup_mode);
    setup_mode_timer.reset();
    blink_timer.reset();
    return;
  }

  // do nothing if not in setup mode
  if (!in_setup_mode) {
    return;
  }

  // toggle edit mode
  if (PRESSED(SetupKeys::Select)) {
    in_edit_mode = !in_edit_mode;
    return;
  }

  // left and right
  int8_t delta = 0;
  delta -= PRESSED(SetupKeys::Left) ? 1 : 0;
  delta += PRESSED(SetupKeys::Right) ? 1 : 0;

  // switch through setup items
  if (!in_edit_mode) {
    item_idx += delta;
    if (item_idx >= item_count) {
      item_idx = 0;
    } else if (item_idx < 0) {
      item_idx = item_count - 1;
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

  // LED1 indicates menu item
  bool led1 = get_led_blink(blink_elapsed, item_idx + 1);

  // LED2 indicates menu item value
  uint8_t item_value = get_item_value();
  bool led2 = get_led_blink(blink_elapsed, item_value);
  // many people can't count to five
  if (item_value > 4) {
    led2 = true;  // display as max value
  }

  // update led state
  set_led_state(led1, led2);
}

SetupKeys SetupMode::get_key_state() {
  SetupKeys keys = SetupKeys::None;

  KeyboardModifierState kbd_modifiers = keyboard->pop_modifier_state();
  if (kbd_modifiers ==
      (KeyboardModifierState::ModLeftCtrl | KeyboardModifierState::ModLeftShift |
       KeyboardModifierState::ModLeftGUI)) {
    keys = (SetupKeys)(keys | SetupKeys::Setup);
  } else {
    keyboard->update_modifier_state(kbd_modifiers);
  }

  if (joystick_state->buttons[5] && joystick_state->buttons[4]) {
    keys = (SetupKeys)(keys | SetupKeys::Setup);
  }

  // don't consume keys if not in setup mode
  if (!in_setup_mode) {
    return keys;
  }

  KeyboardCodes keycode = keyboard->deq_make();
  if (keycode != KeyboardCodes::NoKey) {
    if (keycode == KeyboardCodes::Return) {
      keys = (SetupKeys)(keys | SetupKeys::Select);
    }
    if (keycode == KeyboardCodes::LeftArrow) {
      keys = (SetupKeys)(keys | SetupKeys::Left);
    }
    if (keycode == KeyboardCodes::RightArrow) {
      keys = (SetupKeys)(keys | SetupKeys::Right);
    }

    if (keys == SetupKeys::None) {
      keyboard->enq_make(keycode);
    } else if (keys != SetupKeys::Setup) {
      keyboard->deq_brk();
    }
  }

  if (joystick_state->buttons[0] || joystick_state->buttons[1]) {
    keys = (SetupKeys)(keys | SetupKeys::Select);
  }
  if (joystick_state->buttons[5]) {
    keys = (SetupKeys)(keys | SetupKeys::Left);
  }
  if (joystick_state->buttons[4]) {
    keys = (SetupKeys)(keys | SetupKeys::Right);
  }

  return keys;
}

void SetupMode::set_led_state(bool led1, bool led2) {
  digitalWrite(EXT_LED1_PIN, (led1 | in_edit_mode) ? LOW : HIGH);
  digitalWrite(EXT_LED2_PIN, led2 ? LOW : HIGH);

  KeyboardLeds kbd_leds = KeyboardLeds::LedNone;
  if (led1) kbd_leds = (KeyboardLeds)(kbd_leds | KeyboardLeds::LedCapsLock);
  if (led2) kbd_leds = (KeyboardLeds)(kbd_leds | KeyboardLeds::LedScrollLock);
  if (in_edit_mode) kbd_leds = (KeyboardLeds)(kbd_leds | KeyboardLeds::LedNumLock);
  keyboard->pop_led_state();
  keyboard->update_led_state(kbd_leds);
}

uint8_t SetupMode::get_item_value() {
  switch (item_idx) {
    case 0:
      return swap_joy_axis_3_and_4 ? UINT8_MAX : 0;
    default:
      return dummy_value < 0 ? -dummy_value : dummy_value;
  }
}

void SetupMode::set_item_value(int8_t delta) {
  switch (item_idx) {
    case 0:
      swap_joy_axis_3_and_4 = !swap_joy_axis_3_and_4;
      break;
    default:
      dummy_value += delta;
      break;
  }
}

static bool get_led_blink(unsigned long t_delta, uint8_t count) {
  unsigned long window = SETUP_BLINK_WINDOW / 2;
  if (count < 1 || t_delta > window) {
    return false;
  }

  unsigned long blink_duration = (window * 2) / count;
  unsigned long blink_phase = (t_delta * 2) % blink_duration;
  return blink_phase > (blink_duration / 2);
}
