#include "HIDDescriptionParser.h"

HIDDescriptionParser::HIDDescriptionParser(USBHost *usb) : usb(usb){};

uint8_t HIDDescriptionParser::getUsageDescriptionCount() {
  return usageDescriptionCount;
}

HIDReportUsageDescription const *const HIDDescriptionParser::getUsageDescription(
    uint8_t idx) {
  return &usageDescriptions[idx % usageDescriptionCount];
}

static uint8_t scale_value(uint16_t value,
                           HIDReportUsageDescription const *const description);

uint32_t HIDDescriptionParser::parseHidDataBlock(
    HIDReportUsageDescription const *const description, uint8_t const *const input,
    uint32_t *const inputIdx, uint8_t *const bitIdx, uint8_t *const output,
    uint32_t outputSize) {
  uint32_t outputLen = 0;

  // in case of placeholder -> ignore incomplete byte
  if (description->placeholder) {
    *bitIdx = 0;
    *inputIdx = (*inputIdx) + 1;
    return 0;
  }

  for (uint8_t idx = 0; idx < description->reportCount; idx++) {
    uint16_t curValue = 0;
    uint8_t bitsRemaining = description->reportSize;
    for (; bitsRemaining > 0; bitsRemaining--) {
      if (input[*inputIdx] & (1 << *bitIdx)) {
        curValue |= 1 << (description->reportSize - bitsRemaining);
      }
      if (*bitIdx >= 7) {
        *bitIdx = 0;
        *inputIdx = (*inputIdx) + 1;
      } else {
        *bitIdx = *bitIdx + 1;
      }
    }
    if (outputLen < outputSize) {
      if (description->usage != 0x39) { /* do not scale HATs */
        output[outputLen] = scale_value(curValue, description);
      } else {
        output[outputLen] = min(curValue, 0xFF);
      }
      ++outputLen;
    }
  }

  return outputLen;
}

uint32_t HIDDescriptionParser::parseDeviceDescriptor(uint8_t address,
                                                     uint8_t endpoint) {
  static const uint16_t BUFFER_SIZE = 128;
  uint8_t receiveBuffer[BUFFER_SIZE];
  uint8_t hidRepDescBuffer[BUFFER_SIZE];
  uint8_t rcode;

  // reset usage descriptors
  usageDescriptionCount = 0;
  for (uint8_t idx = 0; idx < MAX_USAGE_DESCRIPTIONS; idx++) {
    memset(&usageDescriptions[idx], 0, sizeof(HIDReportUsageDescription));
  }

  rcode = usb->getDevDescr(address, endpoint, sizeof(USB_DEVICE_DESCRIPTOR),
                           (uint8_t *)receiveBuffer);
  if (rcode) return rcode;

  USB_DEVICE_DESCRIPTOR *devDesc =
      reinterpret_cast<USB_DEVICE_DESCRIPTOR *>(receiveBuffer);

  USBTRACE("USB_DEVICE_DESCRIPTOR\r\n");
  USBTRACE2("bLength=", devDesc->bLength);
  USBTRACE2("bDescriptorType=", devDesc->bDescriptorType);
  USBTRACE2("bDeviceClass=", devDesc->bDeviceClass);
  USBTRACE2("idVendor=", devDesc->idVendor);
  USBTRACE2("idProduct=", devDesc->idProduct);
  USBTRACE2("bNumConfigurations=", devDesc->bNumConfigurations);
  USBTRACE("\r\n");

  for (uint8_t configIdx = 0; configIdx < devDesc->bNumConfigurations; configIdx++) {
    rcode =
        usb->getConfDescr(address, endpoint, sizeof(USB_CONFIGURATION_DESCRIPTOR),
                          configIdx, (uint8_t *)receiveBuffer);
    if (rcode) return rcode;

    USB_CONFIGURATION_DESCRIPTOR *confDesc =
        reinterpret_cast<USB_CONFIGURATION_DESCRIPTOR *>(receiveBuffer);

    USBTRACE("USB_CONFIGURATION_DESCRIPTOR\r\n");
    USBTRACE2("bLength=", confDesc->bLength);
    USBTRACE2("bDescriptorType=", confDesc->bDescriptorType);
    USBTRACE2("wTotalLength=", confDesc->wTotalLength);
    USBTRACE2("bNumInterfaces=", confDesc->bNumInterfaces);
    USBTRACE("\r\n");

    uint16_t totalLength = min(confDesc->wTotalLength, BUFFER_SIZE);
    rcode =
        usb->getConfDescr(address, endpoint, totalLength, configIdx, receiveBuffer);
    if (rcode) return rcode;

    uint8_t hidInterfaceNumber = 0;

    USB_INTERFACE_DESCRIPTOR *interfaceDesc;
    USB_HID_DESCRIPTOR *hidDesc;
    HID_CLASS_DESCRIPTOR_LEN_AND_TYPE *hidClassDesc;

    uint8_t bufferIdx = confDesc->bLength;
    while ((bufferIdx + 2) < totalLength) {
      uint8_t descLen = receiveBuffer[bufferIdx];
      uint8_t descType = receiveBuffer[bufferIdx + 1];
      if ((bufferIdx + descLen) > totalLength) break;  // not enough memory

      switch (descType) {
        case USB_DESCRIPTOR_INTERFACE: {
          interfaceDesc = reinterpret_cast<USB_INTERFACE_DESCRIPTOR *>(
              &receiveBuffer[bufferIdx]);

          USBTRACE("USB_INTERFACE_DESCRIPTOR\r\n");
          USBTRACE2("bLength=", interfaceDesc->bLength);
          USBTRACE2("bDescriptorType=", interfaceDesc->bDescriptorType);
          USBTRACE2("bInterfaceNumber=", interfaceDesc->bInterfaceNumber);
          USBTRACE2("bNumEndpoints=", interfaceDesc->bNumEndpoints);
          USBTRACE2("bInterfaceClass=", interfaceDesc->bInterfaceClass);
          USBTRACE("\r\n");

          if (interfaceDesc->bInterfaceClass == USB_CLASS_CODE_HID) {
            hidInterfaceNumber = interfaceDesc->bInterfaceNumber;
          }
          break;
        }
        case HID_DESCRIPTOR_HID: {
          hidDesc =
              reinterpret_cast<USB_HID_DESCRIPTOR *>(&receiveBuffer[bufferIdx]);

          USBTRACE("USB_HID_DESCRIPTOR\r\n");
          USBTRACE2("bLength=", hidDesc->bLength);
          USBTRACE2("bDescriptorType=", hidDesc->bDescriptorType);
          USBTRACE2("bNumDescriptors=", hidDesc->bNumDescriptors);
          USBTRACE("\r\n");

          for (uint8_t hidClassDescIdx = 0;
               hidClassDescIdx < hidDesc->bNumDescriptors; hidClassDescIdx++) {
            hidClassDesc = reinterpret_cast<HID_CLASS_DESCRIPTOR_LEN_AND_TYPE *>(
                &receiveBuffer[bufferIdx + 6 +
                               (hidClassDescIdx *
                                sizeof(HID_CLASS_DESCRIPTOR_LEN_AND_TYPE))]);

            USBTRACE("HID_CLASS_DESCRIPTOR\r\n");
            USBTRACE2("bDescrType=", hidClassDesc->bDescrType);
            USBTRACE2("wDescriptorLength=", hidClassDesc->wDescriptorLength);
            USBTRACE("\r\n");

            if (hidClassDesc->bDescrType != HID_DESCRIPTOR_REPORT) {
              break;
            }
            // read (1) using a standard (00) transmission
            // and query an interface (00001)
            uint8_t requestType = 0x81;
            uint8_t request = GET_DESCRIPTOR;
            uint16_t responseLen = min(hidClassDesc->wDescriptorLength, BUFFER_SIZE);
            rcode = usb->ctrlReq(address, endpoint, requestType, request,
                                 hidInterfaceNumber, HID_DESCRIPTOR_REPORT,
                                 hidInterfaceNumber, responseLen, 0,
                                 hidRepDescBuffer, NULL);
            if (rcode) return rcode;

            USBTRACE("HID_DESCRIPTOR_REPORT (BUFFER)\r\n");
            for (uint16_t i = 0; i < responseLen; i++) {
              D_PrintHex(hidRepDescBuffer[i], 0x80);
              if (((i + 1) % 16) == 0) {
                USBTRACE("\r\n");
              } else {
                USBTRACE(" ");
              }
            }
            USBTRACE("\r\n");

            // Parse HID Report Descriptor
            uint8_t usagePage = 0;
            uint8_t usage = 0;
            uint16_t logicalMinimum = 0;
            uint16_t logicalMaximum = 0;
            uint16_t physicalMinimum = 0;
            uint16_t physicalMaximum = 0;
            uint8_t usageMinimum = 0;
            uint8_t usageMaximum = 0;
            uint8_t reportSize = 0;
            uint8_t reportCount = 1;
            for (uint16_t idx = 0; idx < responseLen; idx += 2) {
              uint8_t key = hidRepDescBuffer[idx];
              uint8_t value = hidRepDescBuffer[idx + 1];
              switch (key) {
                case 0x05: {  // usage Page
                  usagePage = value;
                  break;
                }
                case 0x09: {  // usage
                  usage = value;
                  break;
                }
                case 0x15: {  // logical minimum
                  logicalMinimum = value;
                  break;
                }
                case 0x16: {  // 3-byte version
                  logicalMinimum = value | (hidRepDescBuffer[idx + 2] << 8);
                  idx++;
                  break;
                }
                case 0x25: {  // logical maximum
                  logicalMaximum = value;
                  break;
                }
                case 0x26: {  // 3-byte version
                  logicalMaximum = value | (hidRepDescBuffer[idx + 2] << 8);
                  idx++;
                  break;
                }
                case 0x35: {  // physical minimum
                  physicalMinimum = value;
                  break;
                }
                case 0x36: {  // 3-byte version
                  physicalMinimum = value | (hidRepDescBuffer[idx + 2] << 8);
                  idx++;
                  break;
                }
                case 0x45: {  // physical maximum
                  physicalMaximum = value;
                  break;
                }
                case 0x46: {  // 3-byte version
                  physicalMaximum = value | (hidRepDescBuffer[idx + 2] << 8);
                  idx++;
                  break;
                }
                case 0x19: {  // usage minimum
                  usageMinimum = value;
                  break;
                }
                case 0x29: {  // usage maximum
                  usageMaximum = value;
                  break;
                }
                case 0x75: {  // report size
                  reportSize = value;
                  break;
                }
                case 0x95: {  // report count
                  reportCount = value;
                  break;
                }
                case 0x81: {  // input
                  USBTRACE("USB HID USAGE DESCRIPTOR\r\n");
                  USBTRACE2("usagePage=", usagePage);
                  USBTRACE2("usage=", usage);
                  USBTRACE2("logicalMinimum=", logicalMinimum);
                  USBTRACE2("logicalMaximum=", logicalMaximum);
                  USBTRACE2("physicalMinimum=", physicalMinimum);
                  USBTRACE2("physicalMaximum=", physicalMaximum);
                  USBTRACE2("usageMinimum=", usageMinimum);
                  USBTRACE2("usageMaximum=", usageMaximum);
                  USBTRACE2("reportSize=", reportSize);
                  USBTRACE2("reportCount=", reportCount);
                  USBTRACE2("placeholder=", value & 1);
                  USBTRACE("\r\n");

                  if (usageDescriptionCount < MAX_USAGE_DESCRIPTIONS) {
                    usageDescriptions[usageDescriptionCount].usagePage = usagePage;
                    usageDescriptions[usageDescriptionCount].usage = usage;
                    usageDescriptions[usageDescriptionCount].logicalMinimum =
                        logicalMinimum;
                    usageDescriptions[usageDescriptionCount].logicalMaximum =
                        logicalMaximum;
                    usageDescriptions[usageDescriptionCount].physicalMinimum =
                        physicalMinimum;
                    usageDescriptions[usageDescriptionCount].physicalMaximum =
                        physicalMaximum;
                    usageDescriptions[usageDescriptionCount].usageMinimum =
                        usageMinimum;
                    usageDescriptions[usageDescriptionCount].usageMaximum =
                        usageMaximum;
                    usageDescriptions[usageDescriptionCount].reportSize = reportSize;
                    usageDescriptions[usageDescriptionCount].reportCount =
                        reportCount;
                    usageDescriptions[usageDescriptionCount].placeholder = value & 1;
                    usageDescriptionCount++;
                  }

                  usage = 0;
                  usageMinimum = 0;
                  usageMaximum = 0;
                  reportSize = 0;
                  reportCount = 1;
                  break;
                }
              }
            }
          }
          break;
        }
        default: {  // ignore other descriptors
          break;
        }
      }
      bufferIdx += descLen;
    }
  }

  return 0;
}

static uint8_t scale_value(uint16_t value,
                           HIDReportUsageDescription const *const description) {
  uint16_t logicalMinimum = description->logicalMinimum;
  uint16_t logicalMaximum = description->logicalMaximum;
  uint32_t scaleFactor;
  uint32_t scaledValue;

  if (logicalMaximum >= logicalMinimum) {
    scaleFactor = (logicalMaximum - logicalMinimum) + 1;
    value = value - logicalMinimum;
  } else {
    value = logicalMinimum - value;
    scaleFactor = (logicalMinimum - logicalMaximum) + 1;
  }

  scaledValue = (((uint32_t)value * UINT8_MAX) + (scaleFactor >> 1)) / scaleFactor;

  return (uint8_t)scaledValue;
}
