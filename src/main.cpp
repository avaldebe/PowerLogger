/*
Based on
- DisplayReadings example fom INA library
  https://github.com/SV-Zanshin/INA/wiki
*/

#include <Arduino.h>
#include "logger.h"
#include <SdFat.h>
ArduinoOutStream cout(Serial);            // stream to Serial
SdFat SD;                                 // File system object.
const uint8_t SD_chipSelect = 10;

void setup() {
  LOGbegin();       // discover & config INA devices
  LOGheader(&cout); // list devices found
}

void loop() {
  if(LOGsave()){    // new set of measurements on buffer
    delay(5000);    
  } else {          // buffer is full
    LOGdump(&cout); // empty buffer
  }
}
