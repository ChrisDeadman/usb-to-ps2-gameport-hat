Usb => PS/2 & Gameport HAT v1.2
===============================

## Where to order which parts?
You can generate a BOM (bill of materials) from within KiCad in order to easily see the parts you need.

I made sure to pick parts with good availability so I could order all of them at once at [Mouser](https://mouser.com).

## Where to order the PCB?
Pretty much all manufacturers should support KiCad or Gerber formats.

I ordered at [EuroCircuits](https://be.eurocircuits.com/) for this project but e.g. [JLCPCB](https://jlcpcb.com/) should also be fine!

## How to generate gerber files?
I pre-generated the gerber files already, you can find them [here](https://github.com/ChrisDeadman/usb-to-ps2-gameport-hat/tree/master/Hardware/usb-to-ps2-gameport-hat-arduino-pro-mini/gerber).

Should you want to generate them yourself, you need to install [KiCad](https://kicad-pcb.org/) to open the project and export the gerber files,
[JLCPCB](https://support.jlcpcb.com/article/44-how-to-export-kicad-pcb-to-gerber-files) has a nice tutorial of how to do that.

## How to solder?
If you order a stencil with your PCB you can use a normal oven to solder the stuff in place - in this case I would still solder the "big" elco manually afterwards because of heat concerns.

I did solder everything manually tho using the soldering iron I use for normal prototyping, just make sure to use a lot of flux for the ICs to avoid shorting the pins - and best triple-check afterwards that you didn't ;)
