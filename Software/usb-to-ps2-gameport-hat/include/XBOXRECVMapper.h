#ifndef _XBOXRECV_MAPPER_H_
#define _XBOXRECV_MAPPER_H_

#include <XBOXRECV.h>
#include "JoystickDriverMapper.h"

class XBOXRECVMapper : public JoystickDriverMapper {
 private:
  XBOXRECV* const driver;
  JoystickState state[4];

 public:
  XBOXRECVMapper(XBOXRECV* driver);

  uint8_t getNumConnectedDevices() override;

  JoystickState getControllerState(uint8_t idx) override;
};

#endif  //_XBOXRECV_MAPPER_H_
