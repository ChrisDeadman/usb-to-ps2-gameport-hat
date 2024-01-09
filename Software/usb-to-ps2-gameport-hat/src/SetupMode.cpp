#include "SetupMode.h"

#include "Config.h"

SetupMode::SetupMode(JoystickState* const joystick_state)
    : joystick_state(joystick_state) {
  key_state = SetupKeys::None;
  item_idx = 0;
  in_setup_mode = false;
  swap_joy_axis_3_and_4 = true;
}

void SetupMode::task() {
  setup_timer.tick();
  blink_timer.tick();

  // menu buttons
  SetupKeys old_key_state = key_state;
  key_state = get_key_state();

  // setup mode is triggered by holding down Button5 + Button6 for a second
  if (!((key_state & SetupKeys::Left) && (key_state & SetupKeys::Right))) {
    setup_timer.reset();
  }
  if (setup_timer.getElapsedMillis() > SETUP_DELAY) {
    in_setup_mode = !in_setup_mode;
    in_edit_mode = false;

    digitalWrite(EXT_LED1_PIN, HIGH);
    digitalWrite(EXT_LED2_PIN, HIGH);

    setup_timer.reset();
    blink_timer.reset();
    return;
  }

  if (!in_setup_mode) {
    return;
  }

  // LED1 is blinking in edit mode
  uint8_t led1_pwm = HIGH;
  if (blink_timer.getElapsedMillis() > SETUP_DELAY) {
    blink_timer.reset();
  }
  // LED1 brightness indicates item index
  if (!in_edit_mode || blink_timer.getElapsedMillis() < (SETUP_DELAY / 2)) {
    led1_pwm = soft_pwm.pwm(item_idx * 0xFF / item_count);
  }
  digitalWrite(EXT_LED1_PIN, led1_pwm);

  // LED2 indicates item value
  digitalWrite(EXT_LED2_PIN, !soft_pwm.pwm(get_item_value()));

  // toggle edit mode
  if (!(old_key_state & SetupKeys::Select) && (key_state & SetupKeys::Select)) {
    in_edit_mode = !in_edit_mode;
    return;
  }

  int8_t delta =
      ((!(old_key_state & SetupKeys::Left) && (key_state & SetupKeys::Left))
           ? -1
           : 0) +
      ((!(old_key_state & SetupKeys::Right) && (key_state & SetupKeys::Right))
           ? 1
           : 0);

  // switch through setup items
  if (!in_edit_mode) {
    item_idx += delta;
    if (item_idx >= item_count) {
      item_idx = 0;
    } else if (item_idx < 0) {
      item_idx = item_count - 1;
    }
    return;
  }

  // update item value
  if (delta) {
    set_item_value(delta);
  }
}

SetupKeys SetupMode::get_key_state() {
  SetupKeys keys = SetupKeys::None;

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

uint8_t SetupMode::get_item_value() {
  switch (item_idx) {
    case 0:
      return swap_joy_axis_3_and_4 ? 0xFF : 0;
    default:
      return 0;
  }
}

void SetupMode::set_item_value(uint8_t delta) {
  switch (item_idx) {
    case 0:
      swap_joy_axis_3_and_4 = !swap_joy_axis_3_and_4;
    default:
      break;
  }
}
