#include "JoystickManager.h"

JoystickManager::JoystickManager(USBHost *usb)
    : xboxRecv(usb),
      hidJoyController1(usb),
      hidJoyController2(usb),
      xboxRecvMapper(&xboxRecv),
      hidJoyController1Mapper(&hidJoyController1),
      hidJoyController2Mapper(&hidJoyController2) {}

uint8_t JoystickManager::getNumConnectedDevices() {
  uint8_t numConnectedDevices = 0;
  for (uint8_t mapper = 0; mapper < numDriverMappers; mapper++) {
    numConnectedDevices += driverMappers[mapper]->getNumConnectedDevices();
  }
  return numConnectedDevices;
}

JoystickState JoystickManager::getControllerState(uint8_t idx) {
  uint8_t deviceStateIdx = 0;
  for (uint8_t mapper = 0; mapper < numDriverMappers; mapper++) {
    uint8_t numDevices = driverMappers[mapper]->getNumConnectedDevices();
    for (uint8_t device = 0; device < numDevices; device++) {
      if (deviceStateIdx == idx) {
        return driverMappers[mapper]->getControllerState(device);
      }
      ++deviceStateIdx;
    }
  }

  // return dummy state if controller is not connected
  JoystickState dummyState;
  return dummyState;
}
