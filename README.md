Usb => PS/2 & Gameport HAT v1.2
===============================

Author: ChrisDeadman

Convert a USB mouse into a PS/2 mouse and/or a USB joystick/gamepad to a gameport joystick with the power of arduino :-)

![3D render](3DRender.png)

![Layout](Layout.png)

## General Notes
* Watchdog timer is enabled and configured to 4sec
* Diagnostic information is available via Serial1 - of course not USB since where else would you plug your devices :D
  Sent/Received data + Status is returned by sending any character to Serial1 @ 115200 8N1
* Debugging messages for USB can be enabled by by uncommenting **#define DEBUG_USB_HOST 1** in **USB.h** (part of the used platform library)
* You need to lower **USB_XFER_TIMEOUT** (to e.g. 500) in **UsbCore.h** (part of the used platform library)
  Usb.Task() may otherwise take longer than the watchdog's timeout
  Also improves compatibility with some HID devices
* Use an USB OTG cable to connect your USB device to the board
* Connect a USB Hub to Arduino if you want to use multiple devices (e.g. USB joystick together with USB mouse)
* Pins can be changed in **include/Config.h**

## PS/2 Notes
* Provides IntelliMouse support (5 buttons + scrollwheel)
* Connect the PS/2 cable for the mouse to J_PS/2_1
* Check [avrfreaks](https://www.avrfreaks.net/sites/default/files/PS2%20Keyboard.pdf) for instruction on how to hook up a connector to it

## Gameport Notes
* Supports generic HID joysticks
* Supports Xbox wireless receiver
* Support for additional devices can be added by implementing a new **JoystickDriverMapper** and adding it to **JoystickManager**
* Depending on the capacitance of your computers gameport it might be neccessary to adapt values in the **AXIS_TO_POT_VALUE** macro.
  It's in **lib/Gameport/Gameport.h** and can be modified to calibrate the hardware for your system.
  Be aware that if the resulting value gets too low it will not be properly detected by some applications (and windows).

## (Known) Limitations
* The hardware provides two PS/2 ports but keyboard support is not implemented in the software
  (altho the included **PS2Device** library can be extended with it)
* The HID descriptor parsing is work in progress so generic HID joystick support may not work for all devices

## Supported Arduino Boards
* [SparkFun SAMD21 Mini Breakout](https://www.sparkfun.com/products/13664)
* Other ATSAMD21G18 boards should also work fine provided they have a compatible Pro-Mini / Pro-Micro pinout 

Low-level documentation about the PS/2 protocol can be found at [avrfreaks](https://www.avrfreaks.net/sites/default/files/PS2%20Keyboard.pdf)

Release notes
=======================

### Usb => PS/2 & Gameport HAT v1.2
* Add a second PS/2 port to support keyboards in the future

### Usb => PS/2 & Gameport HAT v1.1
* Fix incorrect PS/2 pin connections

### Usb => PS/2 & Gameport HAT v1.0
* Initial version
