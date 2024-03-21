#include <Arduino.h>

#include "DeviceEmulation.h"

#include "Config.h"
#include "KeyboardAction.h"

#define CLAMP(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

#define JOY_AXIS_TO_MOUSE_DELTA(axis, speed)                                              \
  (((axis) < JOY_AXIS_CENTER) ? -(((uint16_t)(JOY_AXIS_CENTER - (axis)) << (speed)) >> 6) \
                              : +(((uint16_t)((axis)-JOY_AXIS_CENTER) << (speed)) >> 6))

#define MOUSE_DELTA_TO_JOY_AXIS(delta, speed) \
  (CLAMP(JOY_AXIS_CENTER + (((delta) << (speed)) >> 1), 0, 0xFF))

static KeyBuffer tmp_key_buffer;

static void sync_kb_emu_keys(VirtualKeyboard* const keyboard, KeyboardCodes code, bool key_down);

void joy_emulate_keyboard(VirtualKeyboard* const keyboard,
                          JoystickState const* const old_state,
                          JoystickState const* const new_state) {
  bool key_down;

  // Axes
  for (uint8_t axis_idx = 0; axis_idx < JoystickState::NUM_AXES; axis_idx++) {
    for (int8_t positive = 0; positive < 2; positive++) {
      key_down = JOY_AXIS_TEST(new_state->axes[axis_idx], positive);
      if (key_down != JOY_AXIS_TEST(old_state->axes[axis_idx], positive)) {
        sync_kb_emu_keys(keyboard, KB_EMU_AXIS_MAPPINGS[(axis_idx << 1) + positive][0], key_down);
      }
    }
  }

  // Buttons
  for (uint8_t button = 0; button < JoystickState::NUM_BUTTONS; button++) {
    key_down = new_state->buttons[button] > 0;
    if (key_down != (old_state->buttons[button] > 0)) {
      sync_kb_emu_keys(keyboard, KB_EMU_BTN_MAPPINGS[button][0], key_down);
    }
  }
}

void joy_emulate_mouse(VirtualMouse* const mouse,
                       JoystickState const* const new_state,
                       uint8_t mouse_emu_speed) {
  int16_t d_x;
  int16_t d_y;
  uint8_t button_state;
  MouseState new_mouse_state = mouse->pop_state();
  if (!new_mouse_state.changed) {
    new_mouse_state.d_x = 0;
    new_mouse_state.d_y = 0;
    new_mouse_state.d_wheel = 0;
  }

  // X- & Y-Axis 1
  d_x = JOY_AXIS_TO_MOUSE_DELTA(new_state->axes[0], mouse_emu_speed);
  d_y = JOY_AXIS_TO_MOUSE_DELTA(new_state->axes[1], mouse_emu_speed);
  if (d_x != 0 || d_y != 0) {
    new_mouse_state.d_x = d_x;
    new_mouse_state.d_y = d_y;
    new_mouse_state.changed = true;
  }

  // Y-Axis 2
  if (JOY_AXIS_TEST(new_state->axes[3], false)) {
    new_mouse_state.d_wheel = CLAMP(mouse_emu_speed >> 1, 1, 7);;
    new_mouse_state.changed = true;
  } else if (JOY_AXIS_TEST(new_state->axes[3], true)) {
    new_mouse_state.d_wheel = -CLAMP(mouse_emu_speed >> 1, 1, 7);;
    new_mouse_state.changed = true;
  }

  // Buttons
  if (new_state->changed) {
    for (uint8_t button = 0; button < MouseState::NUM_BUTTONS; button++) {
      button_state = new_state->buttons[button];
      if (button_state != new_mouse_state.buttons[button]) {
        new_mouse_state.buttons[button] = button_state;
        new_mouse_state.changed = true;
      }
    }
  }

  // push back updated state
  mouse->update_state(&new_mouse_state);
}

void keyboard_emulate_joy(VirtualJoystick* const joystick, VirtualKeyboard* const keyboard) {
  JoystickState new_joy_state = joystick->pop_state();

  // prepare temporary key buffer
  tmp_key_buffer.clear();

  // for each keyboard action
  while (true) {
    KeyboardAction kb_action = keyboard->deq();
    if (kb_action.type == KbActionNone) {
      break;
    }
    // don't consume key
    tmp_key_buffer.enq(kb_action);

    //
    // Axis handling
    //
    uint8_t axis_mapping_idx;
    for (axis_mapping_idx = 0; axis_mapping_idx < (NUM_KB_EMU_AXIS_MAPPINGS << 1);
         axis_mapping_idx++) {
      if (kb_action.code == KB_EMU_AXIS_MAPPINGS[axis_mapping_idx >> 1][axis_mapping_idx % 2]) {
        break;
      }
    }
    axis_mapping_idx >>= 1;

    if (axis_mapping_idx < NUM_KB_EMU_AXIS_MAPPINGS) {
      // MAKE
      if (kb_action.type == KbActionMake) {
        if (!JOY_AXIS_TEST(new_joy_state.axes[axis_mapping_idx >> 1], axis_mapping_idx % 2)) {
          JOY_AXIS_SET(new_joy_state.axes[axis_mapping_idx >> 1], axis_mapping_idx % 2);
          new_joy_state.changed = true;
        }
      }
      // BREAK
      else {
        if (new_joy_state.axes[axis_mapping_idx >> 1] != JOY_AXIS_CENTER) {
          new_joy_state.axes[axis_mapping_idx >> 1] = JOY_AXIS_CENTER;
          new_joy_state.changed = true;
        }
      }
    }

    //
    // Button handling
    //
    uint8_t btn_mapping_idx;
    for (btn_mapping_idx = 0; btn_mapping_idx < (NUM_KB_EMU_BTN_MAPPINGS << 1); btn_mapping_idx++) {
      if (kb_action.code == KB_EMU_BTN_MAPPINGS[btn_mapping_idx >> 1][btn_mapping_idx % 2]) {
        break;
      }
    }
    btn_mapping_idx >>= 1;

    if ((axis_mapping_idx >= NUM_KB_EMU_AXIS_MAPPINGS) &&
        (btn_mapping_idx < NUM_KB_EMU_BTN_MAPPINGS)) {
      // MAKE
      if (kb_action.type == KbActionMake) {
        if (new_joy_state.buttons[btn_mapping_idx] < 1) {
          new_joy_state.buttons[btn_mapping_idx] = 1;
          new_joy_state.changed = true;
        }
      }
      // BREAK
      else {
        if (new_joy_state.buttons[btn_mapping_idx] != 0) {
          new_joy_state.buttons[btn_mapping_idx] = 0;
          new_joy_state.changed = true;
        }
      }
    }
  }

  // restore actions we did not consume
  while (tmp_key_buffer.length() > 0) {
    keyboard->enq(tmp_key_buffer.deq());
  }

  // push back updated state
  joystick->update_state(&new_joy_state, true, false);
}

void keyboard_emulate_mouse(VirtualMouse* const mouse,
                            VirtualKeyboard* const keyboard,
                            uint8_t mouse_emu_speed) {
  MouseState new_mouse_state = mouse->pop_state();

  // prepare temporary key buffer
  tmp_key_buffer.clear();

  // for each keyboard action
  while (true) {
    KeyboardAction kb_action = keyboard->deq();
    if (kb_action.type == KbActionNone) {
      break;
    }
    // don't consume key
    tmp_key_buffer.enq(kb_action);

    //
    // Axis handling
    //
    uint8_t axis_mapping_idx;
    for (axis_mapping_idx = 0; axis_mapping_idx < (NUM_KB_EMU_AXIS_MAPPINGS << 1);
         axis_mapping_idx++) {
      if (kb_action.code == KB_EMU_AXIS_MAPPINGS[axis_mapping_idx >> 1][axis_mapping_idx % 2]) {
        break;
      }
    }
    axis_mapping_idx >>= 1;

    // MAKE
    if (kb_action.type == KbActionMake) {
      switch (axis_mapping_idx) {
        case 0:  // Left
          new_mouse_state.d_x = -(1 << mouse_emu_speed);
          new_mouse_state.changed = true;
          break;
        case 1:  // Right
          new_mouse_state.d_x = (1 << mouse_emu_speed);
          new_mouse_state.changed = true;
          break;
        case 2:  // Up
          new_mouse_state.d_y = -(1 << mouse_emu_speed);
          new_mouse_state.changed = true;
          break;
        case 3:  // Down
          new_mouse_state.d_y = (1 << mouse_emu_speed);
          new_mouse_state.changed = true;
          break;
        case 6:  // Scroll Up
          new_mouse_state.d_wheel = CLAMP(mouse_emu_speed >> 1, 1, 7);
          new_mouse_state.changed = true;
          break;
        case 7:  // Scroll Down
          new_mouse_state.d_wheel = -CLAMP(mouse_emu_speed >> 1, 1, 7);
          new_mouse_state.changed = true;
          break;
      }
    }
    // BREAK
    else {
      switch (axis_mapping_idx) {
        case 0:  // Left
          if (new_mouse_state.d_x < 0) {
            new_mouse_state.d_x = 0;
            new_mouse_state.changed = true;
          }
          break;
        case 1:  // Right
          if (new_mouse_state.d_x > 0) {
            new_mouse_state.d_x = 0;
            new_mouse_state.changed = true;
          }
          break;
        case 2:  // Up
          if (new_mouse_state.d_y < 0) {
            new_mouse_state.d_y = 0;
            new_mouse_state.changed = true;
          }
          break;
        case 3:  // Down
          if (new_mouse_state.d_y > 0) {
            new_mouse_state.d_y = 0;
            new_mouse_state.changed = true;
          }
          break;
        case 6:  // Scroll Up
          if (new_mouse_state.d_wheel > 0) {
            new_mouse_state.d_wheel = 0;
            new_mouse_state.changed = true;
          }
          break;
        case 7:  // Scroll Down
          if (new_mouse_state.d_wheel < 0) {
            new_mouse_state.d_wheel = 0;
            new_mouse_state.changed = true;
          }
          break;
      }
    }

    //
    // Button handling
    //
    uint8_t btn_mapping_idx;
    for (btn_mapping_idx = 0; btn_mapping_idx < (NUM_KB_EMU_BTN_MAPPINGS << 1); btn_mapping_idx++) {
      if (kb_action.code == KB_EMU_BTN_MAPPINGS[btn_mapping_idx >> 1][btn_mapping_idx % 2]) {
        break;
      }
    }
    btn_mapping_idx >>= 1;

    if ((axis_mapping_idx >= NUM_KB_EMU_AXIS_MAPPINGS) &&
        (btn_mapping_idx < NUM_KB_EMU_BTN_MAPPINGS)) {
      // MAKE
      if (kb_action.type == KbActionMake) {
        if (new_mouse_state.buttons[btn_mapping_idx] < 1) {
          new_mouse_state.buttons[btn_mapping_idx] = 1;
          new_mouse_state.changed = true;
        }
      }
      // BREAK
      else {
        if (new_mouse_state.buttons[btn_mapping_idx] != 0) {
          new_mouse_state.buttons[btn_mapping_idx] = 0;
          new_mouse_state.changed = true;
        }
      }
    }
  }

  // restore actions we did not consume
  while (tmp_key_buffer.length() > 0) {
    keyboard->enq(tmp_key_buffer.deq());
  }

  // keep velocity
  if ((new_mouse_state.d_x != 0) || (new_mouse_state.d_y != 0) || (new_mouse_state.d_wheel != 0)) {
    new_mouse_state.changed = true;
  }

  // push back updated state
  mouse->update_state(&new_mouse_state);
}

void mouse_emulate_joy(VirtualJoystick* const joystick,
                       MouseState const* const new_state,
                       uint8_t mouse_emu_speed) {
  uint8_t axis;
  uint8_t button_state;
  JoystickState new_joy_state = joystick->pop_state();

  // X-Axis 1
  axis = MOUSE_DELTA_TO_JOY_AXIS(new_state->changed ? new_state->d_x : 0, mouse_emu_speed);
  if ((axis != JOY_AXIS_CENTER) && (axis != new_joy_state.axes[0])) {
    new_joy_state.axes[0] = axis;
    new_joy_state.changed = true;
  }

  // Y-Axis 1
  axis = MOUSE_DELTA_TO_JOY_AXIS(new_state->changed ? new_state->d_y : 0, mouse_emu_speed);
  if ((axis != JOY_AXIS_CENTER) && (axis != new_joy_state.axes[1])) {
    new_joy_state.axes[1] = axis;
    new_joy_state.changed = true;
  }

  // Y-Axis 2
  axis = MOUSE_DELTA_TO_JOY_AXIS(new_state->changed ? new_state->d_wheel : 0, mouse_emu_speed);
  axis = combine_axes(new_joy_state.axes[3], axis);
  if (axis != new_joy_state.axes[3]) {
    new_joy_state.axes[3] = axis;
    new_joy_state.changed = true;
  }

  // Buttons
  if (new_state->changed) {
    for (uint8_t button = 0; button < MouseState::NUM_BUTTONS; button++) {
      button_state = new_state->buttons[button];
      if (button_state != new_joy_state.buttons[button]) {
        new_joy_state.buttons[button] = button_state;
        new_joy_state.changed = true;
      }
    }
  }

  // push back updated state
  joystick->update_state(&new_joy_state, true, false);
}

static void sync_kb_emu_keys(VirtualKeyboard* const keyboard, KeyboardCodes code, bool key_down) {
  if (code == NoKey) {
    return;
  }

  KeyboardAction kb_action;
  kb_action.type = key_down ? KbActionMake : KbActionBreak;
  kb_action.code = code;
  keyboard->enq(kb_action);
}
