#ifndef _PS2_DEVICE_H_
#define _PS2_DEVICE_H_

#include "PS2Port.h"
#include "PS2PortObserver.h"
#include "PS2Receiver.h"
#include "PS2Sender.h"

const uint8_t ACK_CODE = 0xFA;
const uint8_t RESEND_CODE = 0xFE;
const uint8_t BAT_OK = 0xAA;

/**
 * implement in your code if you want to capture packages.
 */
extern void ps2_data_received(uint8_t pin, uint8_t data_byte, bool valid);

/**
 * implement in your code if you want to capture packages.
 */
extern void ps2_data_sent(uint8_t pin, uint8_t data_byte);

class PS2Device : PS2PortObserver {
 private:
  volatile unsigned long time_last_inhibit;
  volatile unsigned long time_last_host_rts;

 protected:
  PS2Sender sender;
  PS2Receiver receiver;
  uint8_t send_buffer[8];
  uint8_t send_buffer_idx;
  uint8_t send_buffer_len;

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
  volatile void on_clock() override;

  /**
   * Called when the host inhibits communication.
   */
  volatile void on_inhibit() override;

  /**
   * Called when the host requests to send.
   */
  volatile void on_host_rts() override;

 protected:
  /**
   * Triggers resending of data upon next clock cycle.
   *
   * This has no effect if no data is currently being sent.
   */
  void resend();

  /**
   * Queue data for sending.
   */
  void send_toHost(const uint8_t* data, uint8_t length);
};

#endif  //_PS2_DEVICE_H_
