#ifndef _XBOXRECV_MAPPER_H_
#define _XBOXRECV_MAPPER_H_

#include <XBOXRECV.h>

#include "JoystickDriverMapper.h"

class XBOXRECVMapper : public IJoystickDriverMapper {
 private:
  XBOXRECV* const driver;
  JoystickState state[4];

  JoystickLeds led_state;
  uint8_t current_led_idx;

 public:
  XBOXRECVMapper(XBOXRECV* driver);

  uint8_t get_num_connected_devices() override;

  JoystickState pop_state(uint8_t idx) override;

  void set_led_state(JoystickLeds new_state) override;
};

#endif  //_XBOXRECV_MAPPER_H_
