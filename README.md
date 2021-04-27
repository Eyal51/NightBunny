# NightBunny

The purpose of this project is to replace the PCB in the bottom of the night lamp bunny bought from Ali with RGB one, powered by Wemos D1 mini.
The goal is to use the RGB lights and programing of the Wemos to allow for toddlers to understand different times of the day without reading a clock.

[![Bunny](https://raw.githubusercontent.com/Eyal51/NightBunny/main/nightbunny.jpeg "Bunny")](https://raw.githubusercontent.com/Eyal51/NightBunny/main/nightbunny.jpeg "Bunny")
[![Circuit](https://raw.githubusercontent.com/Eyal51/NightBunny/main/circuit%20working.jpeg "Circuit")](https://raw.githubusercontent.com/Eyal51/NightBunny/main/circuit%20working.jpeg "Circuit")

For example:
- During the night time the bunny is Red, which means sleep time. This also allows for a night light which does not interfere with night vision for both toddlers and parents
- When its time to wake up the bunny lights in colorful lights - the joy of starting a new day
  
The wemos is programed to OTA updates to allow for easy changes to the firmware, hours, colors etc. without the need to open and disassemble. It also hosts a webserver for logs.
The hours are counted by the Wemos, but updated through NTP once a day.
BTW - NTP is mandatory, and if no WiFi or NTP - no lights.
I could have gone for the RTC route, but this one seemed more complicated, so ¯\\_(ツ)\_/¯

## Circuit

Designed in EasyEDA and manufactured on JLCPCB. In the source files theres the schematics and the JSON for the PCB in EasyEDA.

[![PCB](https://raw.githubusercontent.com/Eyal51/NightBunny/main/PCB_design.png.png "PCB")](https://raw.githubusercontent.com/Eyal51/NightBunny/main/PCB_design.png.png "PCB")

## Installation

Change the WiFi settings (lines 12,13) accordingly, and adjust the UTC offset (line 42). In the code's case it's UTC +3.

## Hardware

- 16 x 5050 RGB SMD Led
- 1 x Wemos D1 Mini or whatever
- 2 x Headers to fit
- 1 x DC barrel jack 5.5 X 2.1mm to be soldered from the underside


------------
If any sources are required just send a message.

