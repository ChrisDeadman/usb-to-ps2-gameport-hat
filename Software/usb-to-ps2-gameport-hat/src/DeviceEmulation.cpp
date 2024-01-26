#include "DeviceEmulation.h"

#include "Config.h"

#define JOY_AXIS_TO_KEYPRESS(axis, positive) \
  (((positive) > 0) ? ((axis) > 0xA0) : ((axis) < 0x60))

#define JOY_AXIS_TO_MOUSE_DELTA(axis) \
  (((axis) < 0x80) ? -((0x80 - (axis)) >> 3) : +(((axis)-0x80) >> 3))

#define MOUSE_DELTA_TO_JOY_AXIS(delta) (0x80 + (delta))

static CircularBuffer<KeyboardCodes, VIRTUAL_KEYBOARD_KRO> make_buffer;
static CircularBuffer<KeyboardCodes, VIRTUAL_KEYBOARD_KRO> brk_buffer;

static void sync_kb_emu_keys(VirtualKeyboard* const keyboard,
                             const KeyboardCodes* kb_codes, bool key_down);

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
  for (uint8_t button_idx = 0; button_idx < JoystickState::NUM_BUTTONS;
       button_idx++) {
    key_down = new_state->buttons[button_idx] > 0;
    if (key_down != (old_state->buttons[button_idx] > 0)) {
      sync_kb_emu_keys(keyboard, KB_EMU_MAPPINGS[mapping_idx][0], key_down);
    }
    ++mapping_idx;
  }
}

void joy_emulate_mouse(VirtualMouse* const mouse,
                       JoystickState const* const new_state) {
  int8_t delta;
  uint8_t button_state;
  MouseState new_mouse_state = mouse->pop_state();

  // X-Axis 1
  delta = JOY_AXIS_TO_MOUSE_DELTA(new_state->axes[0]);
  if (delta != 0) {
    new_mouse_state.d_x = delta;
    new_mouse_state.changed = true;
  }

  // Y-Axis 1
  delta = JOY_AXIS_TO_MOUSE_DELTA(new_state->axes[1]);
  if (delta != 0) {
    new_mouse_state.d_y = delta;
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
  for (uint8_t button_idx = 0; button_idx < MouseState::NUM_BUTTONS; button_idx++) {
    button_state = new_state->buttons[button_idx];
    if (button_state != new_mouse_state.buttons[button_idx]) {
      new_mouse_state.buttons[button_idx] = button_state;
      new_mouse_state.changed = true;
    }
  }

  // push back updated state
  mouse->update_state(&new_mouse_state);
}

void keyboard_emulate_joy(VirtualJoystick* const joystick,
                          VirtualKeyboard* const keyboard) {
  KeyboardCodes make;
  KeyboardCodes brk;
  JoystickState new_joy_state = joystick->pop_state();
  make_buffer.clear();
  brk_buffer.clear();

  // Handle MAKE
  while (true) {
    make = keyboard->deq_make();
    if (make == NoKey) {
      break;
    }

    uint8_t mapping_idx;
    for (mapping_idx = 0; mapping_idx < NUM_KB_EMU_MAPPINGS * 2; mapping_idx++) {
      if (make == KB_EMU_MAPPINGS[mapping_idx / 2][mapping_idx % 2][0]) {
        break;
      }
    }
    mapping_idx /= 2;

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
        make_buffer.enq(make);
        break;
    }
  }

  // restore MAKE codes we did not consume
  while (make_buffer.length() > 0) {
    keyboard->enq_make(make_buffer.deq());
  }

  // Handle BREAK
  while (true) {
    brk = keyboard->deq_brk();
    if (brk == NoKey) {
      break;
    }

    uint8_t mapping_idx;
    for (mapping_idx = 0; mapping_idx < NUM_KB_EMU_MAPPINGS * 2; mapping_idx++) {
      if (brk == KB_EMU_MAPPINGS[mapping_idx / 2][mapping_idx % 2][0]) {
        break;
      }
    }
    mapping_idx /= 2;

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
        brk_buffer.enq(brk);
        break;
    }
  }

  // restore BREAK codes we did not consume
  while (brk_buffer.length() > 0) {
    keyboard->enq_brk(brk_buffer.deq());
  }

  // push back updated state
  joystick->update_state(&new_joy_state, true, false);
}

void keyboard_emulate_mouse(VirtualMouse* const mouse,
                            VirtualKeyboard* const keyboard) {
  KeyboardCodes make;
  KeyboardCodes brk;
  MouseState new_mouse_state = mouse->pop_state();
  make_buffer.clear();
  brk_buffer.clear();

  // Handle MAKE
  while (true) {
    make = keyboard->deq_make();
    if (make == NoKey) {
      break;
    }

    uint8_t mapping_idx;
    for (mapping_idx = 0; mapping_idx < NUM_KB_EMU_MAPPINGS * 2; mapping_idx++) {
      if (make == KB_EMU_MAPPINGS[mapping_idx / 2][mapping_idx % 2][0]) {
        break;
      }
    }
    mapping_idx /= 2;

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
        make_buffer.enq(make);
        break;
    }
  }

  // restore MAKE codes we did not consume
  while (make_buffer.length() > 0) {
    keyboard->enq_make(make_buffer.deq());
  }

  // Handle BREAK
  while (true) {
    brk = keyboard->deq_brk();
    if (brk == NoKey) {
      break;
    }

    uint8_t mapping_idx;
    for (mapping_idx = 0; mapping_idx < NUM_KB_EMU_MAPPINGS * 2; mapping_idx++) {
      if (brk == KB_EMU_MAPPINGS[mapping_idx / 2][mapping_idx % 2][0]) {
        break;
      }
    }
    mapping_idx /= 2;

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
        brk_buffer.enq(brk);
        break;
    }
  }

  // restore BREAK codes we did not consume
  while (brk_buffer.length() > 0) {
    keyboard->enq_brk(brk_buffer.deq());
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
  if (axis != new_joy_state.axes[0]) {
    new_joy_state.axes[0] = axis;
    new_joy_state.changed = true;
  }

  // Y-Axis 1
  axis = MOUSE_DELTA_TO_JOY_AXIS(new_state->changed ? new_state->d_y : 0);
  if (axis != new_joy_state.axes[1]) {
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
  for (uint8_t button_idx = 0; button_idx < MouseState::NUM_BUTTONS; button_idx++) {
    button_state = new_state->buttons[button_idx];
    if (button_state != new_joy_state.buttons[button_idx]) {
      new_joy_state.buttons[button_idx] = button_state;
      new_joy_state.changed = true;
    }
  }

  // push back updated state
  joystick->update_state(&new_joy_state, true, false);
}

static void sync_kb_emu_keys(VirtualKeyboard* const keyboard,
                             const KeyboardCodes* kb_codes, bool key_down) {
  for (uint8_t i = 0; i < 2; i++) {
    if (kb_codes[i] != NoKey) {
      if (key_down) {
        keyboard->enq_make(kb_codes[i]);
      } else {
        keyboard->enq_brk(kb_codes[i]);
      }
    }
  }
}
