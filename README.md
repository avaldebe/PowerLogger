# PowerLogger
DIY multi-channel voltage/current data logger

Inspired by the Power-Meter/Logger project by [GreatScott][],
this project is built around the [INA library][INAlib].

## INAxxx devices

The [INA library][INAlib], allows access to multiple INA219/INA226/INA3221 devices.
As long as each device has a different I2C address, the library will
auto discover them and store the device type and relevant configuration on the microcontroller's EEPROM.

The original [INA library][INAlib], `INAlib` for short, has a large EEPROM footprint (31 bytes/device).
`libs/INA` contains an  `INAlib` [fork][INAfork], `INAfork` for short, which reduces the footprint to 5 bytes/device.

STM32F1 microcontrollers do not have built in EEPROM, so the library provided
with the Arduino core emulates EEPROM with flash memory. This library misses
`get`, `put` and `len` methods required by `INAlib`.
The [fork][INAfork] address this issue.

The patches introduced by the `INAfork` have been submitted for integration, and will hopefully integrated to `INAlib`.

## Buffered SD card
SD card support is povided by the [SdFat library][SdFat].
The [CircularBuffer library][Buffer] provides the buffer for the measurements.

## Builtin RTC (STM32F1)
STM32F1 microcontrollers have a builtin RTC.
The [bluepill][] board has a 32 kHz RTC crystal
and a dedicated pin (marked `VB` on the board and `Vbat` on the pinout)
for a RTC battery backup.

## External RTC (optional)
The following external RTCs are supported via the [RTClib library][RTClib]:
DS1307, DS3231, PCF8583, PCF8563.

## CSV file

Voltage and current measurements are not written directly to the SD card.
They are temporarily sotred on a circular.

The measurements from all INA devices taken "at the same time"
are buffered as one `Record`.
Each `Record` contains single timestamp, and the
voltage and current measurements from each device.
The INA devices are ordered according to their I2C address.

When the buffer is full, each `Record` is written to the SD card
as a single line on a CSV file.

## Install libraries on PlatformIO
```bash
# get INA fork library to lib/INA/
git submodule init lib/INA/

# global install, so it can be used on other projects
pio lib --global install SdFat CircularBuffer

# optional
pio lib --global install RTClib
```

[GreatScott]: https://www.instructables.com/id/Make-Your-Own-Power-MeterLogger/
[bluepill]:   https://wiki.stm32duino.com/index.php?title=Blue_Pill

[INAlib]:  https://github.com/SV-Zanshin/INA
[INAfork]: https://github.com/avaldebe/INA/tree/stm32f1
[SdFat]:   https://github.com/greiman/SdFat
[Buffer]:  https://github.com/rlogiacco/CircularBuffer
[RTClib]:  https://github.com/adafruit/RTClib
