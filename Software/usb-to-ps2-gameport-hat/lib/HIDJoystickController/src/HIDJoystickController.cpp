#include "HIDJoystickController.h"

extern "C" {
void __usb_joy_dummy_received_callback(uint8_t const *const data,
                                       uint8_t length) {}
}
/**
 * implement in your code if you want to capture packages.
 */
void usb_data_received(uint8_t const *const data, uint8_t length)
    __attribute__((weak, alias("__usb_joy_dummy_received_callback")));

HIDJoystickController::HIDJoystickController(USBHost *usb)
    : HIDUniversal::HIDUniversal(usb), usb(usb), hidParser(usb) {
  SetReportParser(0, this);
  memset(state.axes, 0, HIDJoystickControllerState::NUM_AXES);
  memset(state.buttons, 0, HIDJoystickControllerState::NUM_BUTTONS);
  memset(state.hats, 0, HIDJoystickControllerState::NUM_HATS);
}

bool HIDJoystickController::is_connected() { return connected && isReady(); }

HIDJoystickControllerState HIDJoystickController::get_state() { return state; }

void HIDJoystickController::Parse(HID * /* hid */, uint32_t /* is_rpt_id */,
                                  uint32_t len, uint8_t *buf) {
  usb_data_received(buf, (uint8_t)len);

  memset(state.axes, 0, HIDJoystickControllerState::NUM_AXES);
  memset(state.buttons, 0, HIDJoystickControllerState::NUM_BUTTONS);
  memset(state.hats, 0, HIDJoystickControllerState::NUM_HATS);

  uint8_t axisIdx = 0;
  uint8_t buttonIdx = 0;
  uint8_t hatIdx = 0;

  uint32_t bufIdx = 0;
  uint8_t bitIdx = 0;

  for (uint8_t idx = 0; idx < hidParser.getUsageDescriptionCount(); idx++) {
    HIDReportUsageDescription const *const desc =
        hidParser.getUsageDescription(idx);

    // axes
    if (desc->usage >= 0x30 && desc->usage <= 0x38) {
      uint32_t outputSize = HIDJoystickControllerState::NUM_AXES - axisIdx;
      axisIdx += hidParser.parseHidDataBlock(desc, buf, &bufIdx, &bitIdx,
                                             &state.axes[axisIdx], outputSize);
    }
    // HAT
    else if (desc->usage == 0x39) {
      uint32_t outputSize = HIDJoystickControllerState::NUM_HATS - hatIdx;
      hatIdx += hidParser.parseHidDataBlock(desc, buf, &bufIdx, &bitIdx,
                                            &state.hats[hatIdx], outputSize);
    }
    // buttons
    else if (desc->usagePage == 0x09) {
      uint32_t outputSize = HIDJoystickControllerState::NUM_BUTTONS - buttonIdx;
      buttonIdx += hidParser.parseHidDataBlock(
          desc, buf, &bufIdx, &bitIdx, &state.buttons[buttonIdx], outputSize);
    }
    // parse but ignore other blocks
    else {
      hidParser.parseHidDataBlock(desc, buf, &bufIdx, &bitIdx, NULL, 0);
    }
  }
  state.version_counter += 1;
};

uint32_t HIDJoystickController::Init(uint32_t parent, uint32_t port,
                                     uint32_t lowspeed) {
  uint32_t rcode = HIDUniversal::Init(parent, port, lowspeed);
  if (rcode) return rcode;

  rcode = hidParser.parseDeviceDescriptor(bAddress, 0);
  if (rcode) {
    Release();
    rcode = USB_DEV_CONFIG_ERROR_DEVICE_NOT_SUPPORTED;
  } else {
    HIDReportUsageDescription const *const desc =
        hidParser.getUsageDescription(0);
    if (desc->usage == 0x04) {
      connected = true;
    } else {
      Release();
      rcode = USB_DEV_CONFIG_ERROR_DEVICE_NOT_SUPPORTED;
    }
  }
  return rcode;
}
