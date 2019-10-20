#ifndef _JOYSTICK_MANAGER_H_
#define _JOYSTICK_MANAGER_H_

#include <Usb.h>
#include <XBOXRECV.h>
#include "HIDJoystickController.h"
#include "HIDJoystickControllerMapper.h"
#include "XBOXRECVMapper.h"

class JoystickManager : public JoystickDriverMapper {
 private:
  // Joystick drivers.
  XBOXRECV xboxRecv;
  HIDJoystickController hidJoyController1;
  HIDJoystickController hidJoyController2;

  // Joystick mappers.
  // Implement additional mappers and add them here to add support for more
  // devices.
  XBOXRECVMapper xboxRecvMapper;
  HIDJoystickControllerMapper hidJoyController1Mapper;
  HIDJoystickControllerMapper hidJoyController2Mapper;

  static const int numDriverMappers = 3;
  JoystickDriverMapper *driverMappers[numDriverMappers] = {
      &xboxRecvMapper, &hidJoyController1Mapper, &hidJoyController2Mapper};

 public:
  JoystickManager(USBHost *usb);

  uint8_t getNumConnectedDevices() override;

  JoystickState getControllerState(uint8_t idx) override;
};

#endif  // _JOYSTICK_MANAGER_H_
