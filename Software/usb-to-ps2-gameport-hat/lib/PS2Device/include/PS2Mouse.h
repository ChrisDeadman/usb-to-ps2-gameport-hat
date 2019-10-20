#ifndef _PS2_MOUSE_H_
#define _PS2_MOUSE_H_

#include "CircularBuffer.h"
#include "PS2Device.h"
#include "PS2MouseState.h"
#include "SoftTimer.h"

#define DEFAULT_SAMPLE_RATE 100
#define DEFAULT_RESOLUTION 4
#define MAX_RESOLUTION 8
#define DEFAULT_SCALING_2X1 false

class PS2Mouse : public PS2Device {
 private:
  const uint8_t ACK_CODE = 0xFA;
  const uint8_t BAT_OK = 0xAA;
  const uint8_t DEVICE_ID_MOUSE_STD = 0x00;
  const uint8_t DEVICE_ID_MOUSE_WHEEL = 0x03;
  const uint8_t DEVICE_ID_MOUSE_WHEEL_5BUTTONS = 0x04;

  uint8_t sendBuffer[5];
  uint8_t sendBufferIdx = 0;
  uint8_t sendBufferLen = 0;

  CircularBuffer<uint8_t, 3> sampleRateHistory;
  SoftTimer sampleTimer;

  uint8_t deviceId;
  uint8_t activeCommand;
  bool streamingMode;
  bool dataReporting;
  uint8_t sampleRate;
  uint8_t resolution;
  bool scaling2x1;

  PS2MouseState state;
  bool mouseStateChanged;

 public:
  PS2Mouse(PS2Port* port);

  /**
   * Returns the current device-id.
   */
  uint8_t getDeviceId();

  /**
   * Applies default values to all settings.
   */
  void setDefaults();

  /**
   * Applies default values and sends a BAT_OK response.
   *
   * If sendAck is set, ACK_CODE is sent before the response.
   */
  void reset(bool sendAck = false);

  /**
   * Updates the mouse state from an external source.
   */
  void updateState(PS2MouseState const* const newState);

  /**
   * PS/2 main task.
   *
   * This should be called periodically in the loop() function.
   */
  void task();

 private:
  void sendToHost(const uint8_t* data, uint8_t len);
  void handleActiveCommand(uint8_t dataByte);
  void handleNewCommand(uint8_t dataByte);
  uint8_t buildStatusPacket(uint8_t* packet);
  uint8_t buildMovementPacket(boolean use2x1Scaling, uint8_t* packet);
  uint8_t apply2x1Scaling(uint8_t movement);
};

#endif  //_PS2_MOUSE_H_
