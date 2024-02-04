#ifndef _HID_JOYSTICK_CONTROLLER_MAPPER_H_
#define _HID_JOYSTICK_CONTROLLER_MAPPER_H_

#include "HIDJoystickController.h"
#include "JoystickDriverMapper.h"

class HIDJoystickControllerMapper : public IJoystickDriverMapper {
 private:
  HIDJoystickController* const driver;

 public:
  HIDJoystickControllerMapper(HIDJoystickController* driver);

  uint8_t get_num_connected_devices() override;

  JoystickState pop_state(uint8_t idx) override;
};

#endif  //_HID_JOYSTICK_CONTROLLER_MAPPER_H_
