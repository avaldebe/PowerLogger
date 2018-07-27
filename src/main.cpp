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
  Serial.begin(115200);
  LOGG.begin();       // discover & config INA devices
  LOGG.header(&cout); // list devices found
}

void loop() {
  if(LOGG.save()){    // new set of measurements on buffer
    delay(5000);    
  } else {          // buffer is full
    LOGG.dump(&cout); // empty buffer
  }
}
