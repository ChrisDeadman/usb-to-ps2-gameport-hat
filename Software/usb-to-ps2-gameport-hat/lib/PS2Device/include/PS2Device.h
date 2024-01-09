#ifndef _PS2_DEVICE_H_
#define _PS2_DEVICE_H_

#include "PS2Port.h"
#include "PS2PortObserver.h"
#include "PS2Receiver.h"
#include "PS2Sender.h"

const uint8_t ACK_CODE = 0xFA;
const uint8_t RESEND_CODE = 0xFE;
const uint8_t BAT_OK = 0xAA;

class PS2Device : PS2PortObserver {
 private:
  unsigned long time_last_inhibit;
  unsigned long time_last_host_rts;

 protected:
  PS2Sender sender;
  PS2Receiver receiver;

 public:
  PS2Device(PS2Port* const port);

  PS2Port* const port;

  /**
   * Initialize this device.
   */
  void init();

  /**
   * Returns whether this device is busy sending or receiving data.
   */
  bool is_busy();

  /**
   * Returns when this device was last inhibited by the host.
   */
  unsigned long get_time_last_inhibit();

  /**
   * Returns when this device received the last RTS by the host.
   */
  unsigned long get_time_last_host_rts();

  /**
   * Called upon each clock cycle. This is the time to send or receive data.
   */
  void on_clock() override;

  /**
   * Called when the host inhibits communication.
   */
  void on_inhibit() override;

  /**
   * Called when the host requests to send.
   */
  void on_host_rts() override;
};

#endif  //_PS2_DEVICE_H_
