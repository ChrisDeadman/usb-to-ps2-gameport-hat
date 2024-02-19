#include "DeviceEmulation.h"

#include "Config.h"
#include "KeyboardAction.h"

#define JOY_AXIS_TO_KEYPRESS(axis, positive) \
  (((positive) > 0) ? ((axis) > 0xA0) : ((axis) < 0x60))

#define JOY_AXIS_TO_MOUSE_DELTA(axis)                                      \
  (((axis) < 0x80) ? -(((uint16_t)(0x80 - (axis)) * MOUSE_EMU_SPEED) >> 6) \
                   : +(((uint16_t)((axis)-0x80) * MOUSE_EMU_SPEED) >> 6))

#define MOUSE_DELTA_TO_JOY_AXIS(delta) (0x80 + (((delta) * MOUSE_EMU_SPEED) >> 1))

static KeyBuffer tmp_key_buffer;

static void sync_kb_emu_keys(VirtualKeyboard* const keyboard, KeyboardCodes code,
                             bool key_down);

void joy_emulate_keyboard(VirtualKeyboard* const keyboard,
                          JoystickState const* const old_state,
                          JoystickState const* const new_state) {
  bool key_down;
  uint8_t mapping_idx = 0;

  // Axes
  for (uint8_t axis_idx = 0; axis_idx < JoystickState::NUM_AXES; axis_idx++) {
    for (int8_t positive = 0; positive < 2; positive++) {
      key_down = JOY_AXIS_TO_KEYPRESS(new_state->axes[axis_idx], positive);
      if (key_down != JOY_AXIS_TO_KEYPRESS(old_state->axes[axis_idx], positive)) {
        sync_kb_emu_keys(keyboard, KB_EMU_MAPPINGS[mapping_idx][0], key_down);
      }
      ++mapping_idx;
    }
  }

  // Buttons
  for (uint8_t button = 0; button < JoystickState::NUM_BUTTONS; button++) {
    key_down = new_state->buttons[button] > 0;
    if (key_down != (old_state->buttons[button] > 0)) {
      sync_kb_emu_keys(keyboard, KB_EMU_MAPPINGS[mapping_idx][0], key_down);
    }
    ++mapping_idx;
  }
}

void joy_emulate_mouse(VirtualMouse* const mouse,
                       JoystickState const* const new_state) {
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
  d_x = JOY_AXIS_TO_MOUSE_DELTA(new_state->axes[0]);
  d_y = JOY_AXIS_TO_MOUSE_DELTA(new_state->axes[1]);
  if (d_x != 0 || d_y != 0) {
    new_mouse_state.d_x = d_x;
    new_mouse_state.d_y = d_y;
    new_mouse_state.changed = true;
  }

  // No X-Axis 2

  // Y-Axis 2
  if (JOY_AXIS_TO_KEYPRESS(new_state->axes[3], true)) {
    new_mouse_state.d_wheel = 1;
    new_mouse_state.changed = true;
  } else if (JOY_AXIS_TO_KEYPRESS(new_state->axes[3], false)) {
    new_mouse_state.d_wheel = -1;
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

void keyboard_emulate_joy(VirtualJoystick* const joystick,
                          VirtualKeyboard* const keyboard) {
  JoystickState new_joy_state = joystick->pop_state();

  // prepare temporary key buffer
  tmp_key_buffer.clear();

  // for each keyboard action
  while (true) {
    KeyboardAction kb_action = keyboard->deq();
    if (kb_action.type == KbActionNone) {
      break;
    }

    uint8_t mapping_idx;
    for (mapping_idx = 0; mapping_idx < NUM_KB_EMU_MAPPINGS * 2; mapping_idx++) {
      if (kb_action.code == KB_EMU_MAPPINGS[mapping_idx / 2][mapping_idx % 2]) {
        break;
      }
    }
    mapping_idx /= 2;

    // handle MAKE
    if (kb_action.type == KbActionMake) {
      switch (mapping_idx) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7: {  // Axes
          if (mapping_idx % 2 == 0) {
            if (new_joy_state.axes[mapping_idx >> 1] > 0x40) {
              new_joy_state.axes[mapping_idx >> 1] = 0x40;
              new_joy_state.changed = true;
            }
          } else {
            if (new_joy_state.axes[mapping_idx >> 1] < 0xC0) {
              new_joy_state.axes[mapping_idx >> 1] = 0xC0;
              new_joy_state.changed = true;
            }
          }
          break;
        }
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15: {  // Buttons 1-8
          if (new_joy_state.buttons[mapping_idx - 8] < 1) {
            new_joy_state.buttons[mapping_idx - 8] = 1;
            new_joy_state.changed = true;
          }
          break;
        }
        default:
          // put back what we don't consume
          tmp_key_buffer.enq(kb_action);
          break;
      }
    }
    // handle BREAK
    else {
      switch (mapping_idx) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7: {  // Axes
          if (new_joy_state.axes[mapping_idx >> 1] != 0x80) {
            new_joy_state.axes[mapping_idx >> 1] = 0x80;
            new_joy_state.changed = true;
          }
          break;
        }
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15: {  // Buttons 1-8
          if (new_joy_state.buttons[mapping_idx - 8] != 0) {
            new_joy_state.buttons[mapping_idx - 8] = 0;
            new_joy_state.changed = true;
          }
          break;
        }
        default:
          // put back what we don't consume
          tmp_key_buffer.enq(kb_action);
          break;
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
                            VirtualKeyboard* const keyboard) {
  MouseState new_mouse_state = mouse->pop_state();

  // prepare temporary key buffer
  tmp_key_buffer.clear();

  // for each keyboard action
  while (true) {
    KeyboardAction kb_action = keyboard->deq();
    if (kb_action.type == KbActionNone) {
      break;
    }

    uint8_t mapping_idx;
    for (mapping_idx = 0; mapping_idx < NUM_KB_EMU_MAPPINGS * 2; mapping_idx++) {
      if (kb_action.code == KB_EMU_MAPPINGS[mapping_idx / 2][mapping_idx % 2]) {
        break;
      }
    }
    mapping_idx /= 2;

    // handle MAKE
    if (kb_action.type == KbActionMake) {
      switch (mapping_idx) {
        case 0:  // Left
          new_mouse_state.d_x = -MOUSE_EMU_SPEED;
          break;
        case 1:  // Right
          new_mouse_state.d_x = MOUSE_EMU_SPEED;
          break;
        case 2:  // Up
          new_mouse_state.d_y = -MOUSE_EMU_SPEED;
          break;
        case 3:  // Down
          new_mouse_state.d_y = MOUSE_EMU_SPEED;
          break;
        case 4:  // Unused
        case 5:  // Unused
          break;
        case 6:  // Scroll Up
          new_mouse_state.d_wheel = -1;
          break;
        case 7:  // Scroll Down
          new_mouse_state.d_wheel = 1;
          break;
        case 8:
        case 9:
        case 10:
        case 11: {  // Buttons 1-4
          if (new_mouse_state.buttons[mapping_idx - 8] < 1) {
            new_mouse_state.buttons[mapping_idx - 8] = 1;
            new_mouse_state.changed = true;
          }
          break;
        }
        default:
          // put back what we don't consume
          tmp_key_buffer.enq(kb_action);
          break;
      }
    }
    // handle BREAK
    else {
      switch (mapping_idx) {
        case 0:  // Left
        case 1:  // Right
          new_mouse_state.d_x = 0;
          break;
        case 2:  // Up
        case 3:  // Down
          new_mouse_state.d_y = 0;
          break;
        case 4:  // Unused
        case 5:  // Unused
          break;
        case 6:  // Scroll Up
        case 7:  // Scroll Down
          new_mouse_state.d_wheel = 0;
          break;
        case 8:
        case 9:
        case 10:
        case 11:  // Buttons 1-4
          if (new_mouse_state.buttons[mapping_idx - 8] != 0) {
            new_mouse_state.buttons[mapping_idx - 8] = 0;
            new_mouse_state.changed = true;
          }
          break;
        default:
          // put back what we don't consume
          tmp_key_buffer.enq(kb_action);
          break;
      }
    }
  }

  // restore actions we did not consume
  while (tmp_key_buffer.length() > 0) {
    keyboard->enq(tmp_key_buffer.deq());
  }

  // push back updated state
  new_mouse_state.changed |= new_mouse_state.d_x != 0;
  new_mouse_state.changed |= new_mouse_state.d_y != 0;
  new_mouse_state.changed |= new_mouse_state.d_wheel != 0;
  mouse->update_state(&new_mouse_state);
}

void mouse_emulate_joy(VirtualJoystick* const joystick,
                       MouseState const* const new_state) {
  uint8_t axis;
  uint8_t button_state;
  JoystickState new_joy_state = joystick->pop_state();

  // X-Axis 1
  axis = MOUSE_DELTA_TO_JOY_AXIS(new_state->changed ? new_state->d_x : 0);
  if (axis != 0x80 && axis != new_joy_state.axes[0]) {
    new_joy_state.axes[0] = axis;
    new_joy_state.changed = true;
  }

  // Y-Axis 1
  axis = MOUSE_DELTA_TO_JOY_AXIS(new_state->changed ? new_state->d_y : 0);
  if (axis != 0x80 && axis != new_joy_state.axes[1]) {
    new_joy_state.axes[1] = axis;
    new_joy_state.changed = true;
  }

  // No X-Axis 2

  // Y-Axis 2
  axis = MOUSE_DELTA_TO_JOY_AXIS(new_state->changed ? new_state->d_wheel : 0);
  axis = VirtualJoystick::combine_axis(new_joy_state.axes[3], axis);
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

static void sync_kb_emu_keys(VirtualKeyboard* const keyboard, KeyboardCodes code,
                             bool key_down) {
  if (code == NoKey) {
    return;
  }

  KeyboardAction kb_action;
  kb_action.type = key_down ? KbActionMake : KbActionBreak;
  kb_action.code = code;
  keyboard->enq(kb_action);
}
