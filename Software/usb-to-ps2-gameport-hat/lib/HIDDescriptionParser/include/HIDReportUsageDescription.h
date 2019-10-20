#ifndef _HID_REPORT_USAGE_DESCRIPTION_H_
#define _HID_REPORT_USAGE_DESCRIPTION_H_

#include <Arduino.h>

struct HIDReportUsageDescription {
  uint8_t usagePage;
  uint8_t usage;
  uint16_t logicalMinimum;
  uint16_t logicalMaximum;
  uint16_t physicalMinimum;
  uint16_t physicalMaximum;
  uint8_t usageMinimum;
  uint8_t usageMaximum;
  uint8_t reportSize;
  uint8_t reportCount;
  uint8_t placeholder;
};

#endif  // _HID_REPORT_USAGE_DESCRIPTION_H_
