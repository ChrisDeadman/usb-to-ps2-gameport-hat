#ifndef _PS2_DEVICE_H_
#define _PS2_DEVICE_H_

#include "PS2Port.h"
#include "PS2PortObserver.h"
#include "PS2Receiver.h"
#include "PS2Sender.h"

class PS2Device : PS2PortObserver {
 private:
  unsigned long timeLastInhibit;
  unsigned long timeLastHostRts;

 protected:
  PS2Sender sender;
  PS2Receiver receiver;

 public:
  PS2Device(PS2Port* port);

  /**
   * Returns whether this device is busy sending or receiving data.
   */
  bool isBusy();

  /**
   * Returns when this device was last inhibited by the host.
   */
  unsigned long getTimeLastInhibit();

  /**
   * Returns when this device received the last RTS by the host.
   */
  unsigned long getTimeLastHostRts();

  void onClock() override;
  void onInhibit() override;
  void onHostRts() override;
};

#endif  //_PS2_DEVICE_H_
