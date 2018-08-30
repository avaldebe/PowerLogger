# PowerLogger
DIY multi-channel voltage/current data logger

Inspired by the Power-Meter/Logger project by [GreatScott][],
this project is built around the [INA library][INAlib].

## INAxxx devices
The [INA library][INAlib], allows access to multiple INA219/INA226/INA3221 devices.
As long as each device has a different I2C address, the library will
auto discover them and store the device type and relevant configuration on the microcontroller's EEPROM.

## Buffered SD card
SD card support is provided by the [SdFat library][SdFat].
The [CircularBuffer library][Buffer] provides the buffer for the measurements.

## Builtin RTC (STM32F1)
STM32F1 microcontrollers have a builtin RTC.
The [bluepill][] board has a 32 kHz RTC crystal
and a dedicated pin (marked `VB` on the board and `Vbat` on the pinout)
for a RTC backup battery.

## External RTC (optional)
The following external RTCs are supported via the [RTClib library][RTClib]:
DS1307, DS3231, PCF8583, PCF8563.

## Display (optional)
Small displays are supported thanks to the [U8x8 constructors][U8x8]
from the [U8g2 library][U8g2]. At the moment only some 128x64 and 128x32 I2C
displays are supported.

When a display is present, the print statements that otherwise would go to
`Serial` are redirected to the display. The measurements are formatted
to fit 6 channels on a 128x64 display and 2 channels on a 128x32 display.

## CSV file
Voltage and current measurements are not written directly to the SD card.
They are temporarily sotred on a circular buffer.

The measurements from all INA devices taken "at the same time"
are buffered as one `Record`.
Each `Record` contains single timestamp, and the
voltage and current measurements from each device.
The INA devices are ordered according to their I2C address.

When the buffer is full, each `Record` is written to the SD card
as a single line on a CSV file.

## Single button pause/resume/shutdown
Besides debouncing, the [OneButton library][OneButton] provides
multiple functionality with a single tact button:
- short press: pause/resume recording
- long press: safe shutdown
- double press: display backlight (if applicable)

Short press will toggle pause/resume recording to the SD card.
The contents of the buffer are written to the SD card before pausing.
Measurements will continue, but they will not be saved to the buffer
until recording is resumed.

Safe shutdown can be implemented with [additional circuitry][softpower].
(for 3.3V replace the IRF7319 by a IRF7317, as sugested [here][battery]).
A long press will write buffet to the SD card and power down the logger.
A subsequent single press will power up the logger.

A double press will switch on/off the display backlight, if applicable.
The switching part of the [shutdown circuit][softpower] (P/N channel MOSFETS)
can be be used to connect/disconnect the display backlight power pin (`BACKLIGHT_PIN` flag).
If no `BACKLIGHT_PIN` flag is defined, the double press will put the display
on [power save mode][U8x8].

# Instructions

## Project configuration
Different hardware combinations are defined on `platformio.ini`.
Other options, such as sampling frequency, number of channels and buffer size,
are defined on  `src/config.h`. Change as needed and copile the project with `pio run`.

**Note** a single INA3221 counts as 3 INA channels.

## Project libraries on lib/
The libraries on `lib/` provide simplified interfaces to the RTC object (`lib/RTCutil`),
display/serial `Print` objects (`lib/TERMutil`),
and the buffering of INA measurements (`lib/INAbufer`).
Each local library contains a `library.json` file describing the
library dependencies and build flags.

## External library dependencies
This project external library dependencies are described
by `lib_deps` on `platformio.ini`, and `dependencies` in `lib/*/library.json`.
The first time the project is compiled,
the [PlatformIO Library Manager][piolib] will download and install all the dependencies.
They will be installed to `~/.platformio/lib`, which is assumed to be the global library path.
For a local install comment the `libdeps_dir` definition on `platformio.ini`.

The following commands are provided for reference.
```bash
# global install, so it can be used on other projects
pio lib --global install INA SdFat CircularBuffer OneButton RTClib U8g2
```

The [MemoryFree library][MemoryFree] is not available on the [Library Manager][piolib], so we need to specify project repository.
```bash
# global install from project repository
pio lib --global install git@github.com:mpflaga/Arduino-MemoryFree.git
```

# Need for help?
## Test the project first
The different components of this project are tested on `test/test_main.cpp`.
The RAM usage is monitored using the [MemoryFree library][MemoryFree].
Please run `pio test` before opening an issue asking for support.

[GreatScott]: https://www.instructables.com/id/Make-Your-Own-Power-MeterLogger/
[bluepill]:   https://wiki.stm32duino.com/index.php?title=Blue_Pill
[softpower]:  http://www.mosaic-industries.com/embedded-systems/microcontroller-projects/electronic-circuits/push-button-switch-turn-on/microcontroller-latching-on-off
[battery]: http://www.mosaic-industries.com/embedded-systems/microcontroller-projects/electronic-circuits/push-button-switch-turn-on/switching-battery-power

[piolib]:  http://docs.platformio.org/en/latest/librarymanager/index.html
[INAlib]:  https://github.com/SV-Zanshin/INA
[INAfork]: https://github.com/avaldebe/INA/tree/stm32f1
[SdFat]:   https://github.com/greiman/SdFat
[Buffer]:  https://github.com/rlogiacco/CircularBuffer
[OneButton]: https://github.com/mathertel/OneButton
[RTClib]:  https://github.com/adafruit/RTClib
[U8g2]:    https://github.com/olikraus/u8g2
[U8x8]:    https://github.com/olikraus/u8g2/wiki/u8x8reference
[MemoryFree]: https://github.com/mpflaga/Arduino-MemoryFree
