#ifndef _HID_JOYSTICK_CONTROLLER_MAPPER_H_
#define _HID_JOYSTICK_CONTROLLER_MAPPER_H_

#include "HIDJoystickController.h"
#include "JoystickDriverMapper.h"

class HIDJoystickControllerMapper : public JoystickDriverMapper {
 private:
  HIDJoystickController* const driver;
  JoystickState state;

 public:
  HIDJoystickControllerMapper(HIDJoystickController* driver);

  uint8_t getNumConnectedDevices() override;

  JoystickState getControllerState(uint8_t idx) override;
};

#endif  //_HID_JOYSTICK_CONTROLLER_MAPPER_H_
