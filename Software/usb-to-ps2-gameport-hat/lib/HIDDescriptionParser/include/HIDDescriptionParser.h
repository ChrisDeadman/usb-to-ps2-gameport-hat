#ifndef _HID_DESCRIPTION_PARSER_H_
#define _HID_DESCRIPTION_PARSER_H_

#include <hid.h>
#include "HIDReportUsageDescription.h"

class HIDDescriptionParser {
 private:
  static const uint8_t USB_CLASS_CODE_HID = 0x03;
  static const uint8_t MAX_USAGE_DESCRIPTIONS = 10;

  USBHost* const usb;

  HIDReportUsageDescription usageDescriptions[MAX_USAGE_DESCRIPTIONS];

  uint8_t usageDescriptionCount = 0;

 public:
  HIDDescriptionParser(USBHost* usb);

  /**
   * Returns the number of parsed usage descriptions.
   */
  uint8_t getUsageDescriptionCount();

  /**
   * Returns a usage description at the specified index.
   */
  HIDReportUsageDescription const* const getUsageDescription(uint8_t idx);

  /**
   * Parses a single data block according to the given usage description and
   * stores the result in the provided output array.
   *
   * Returns the number of output bytes.
   */
  uint32_t parseHidDataBlock(HIDReportUsageDescription const* const description,
                             uint8_t const* const input,
                             uint32_t* const inputIdx, uint8_t* const bitIdx,
                             uint8_t* const output, uint32_t outputSize);

  /**
   * Parses a device descriptor and stores the usage descriptions.
   */
  uint32_t parseDeviceDescriptor(uint8_t address, uint8_t endpoint);
};

#endif  // _HID_DESCRIPTION_PARSER_H_
