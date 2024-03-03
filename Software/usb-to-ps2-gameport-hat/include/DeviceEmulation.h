#ifndef _DEVICE_EMULATION_H_
#define _DEVICE_EMULATION_H_

#include "JoystickState.h"
#include "MouseState.h"
#include "VirtualJoystick.h"
#include "VirtualKeyboard.h"
#include "VirtualMouse.h"

void joy_emulate_keyboard(VirtualKeyboard* const keyboard,
                          JoystickState const* const old_state,
                          JoystickState const* const new_state);

void joy_emulate_mouse(VirtualMouse* const mouse,
                       JoystickState const* const new_state,
                       uint8_t mouse_emu_speed);

void keyboard_emulate_joy(VirtualJoystick* const joystick, VirtualKeyboard* const keyboard);

void keyboard_emulate_mouse(VirtualMouse* const mouse,
                            VirtualKeyboard* const keyboard,
                            uint8_t mouse_emu_speed);

void mouse_emulate_joy(VirtualJoystick* const joystick,
                       MouseState const* const new_state,
                       uint8_t mouse_emu_speed);

#endif /* _DEVICE_EMULATION_H_ */
