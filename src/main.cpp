/*
Based on

- DisplayReadings example fom INA library
  https://github.com/SV-Zanshin/INA

- QuickStart example form the SdFat library
  https://github.com/greiman/SdFat

- Object example on the CircularBuffer library
  https://github.com/rlogiacco/CircularBuffer

- Terminal example from the U8g2 library
  https://github.com/olikraus/u8g2/
*/

#include <Arduino.h>
#include "config.h"                       // project configuration
#include "Record.h"                       // secret sauce ;-)

#include <SdFat.h>
SdFat SD;                                 // File system object.
File CSV;                                 // File object for filename

#include "RTCutil.h"
#ifdef HAST_RTC
  #define FILENAME rtc_fmt('C')           // 'YYMMDD.csv'
#else
  #define FILENAME "INA.csv"
#endif

#include "U8X8util.h"
#ifdef HAST_U8X8
  #define TERNIMAL u8x8log
#else
  #define TERNIMAL Serial
#endif

void setup() {
  Serial.begin(57600);                    // for ATmega328p 3.3V 8Mhz
  while(!Serial){ SysCall::yield(); }     // Wait for USB Serial

  rtc_init(&TERNIMAL);                    // update RTC if needed

  if (!SD.begin(chipSelect, SPI_SPEED)) {
    SD.initErrorHalt(&TERNIMAL);          // errorcode/message to TERNIMAL
  }

  Record::init(&TERNIMAL, FILENAME);      // init/config INA devices
}

void loop() {
  static uint32_t last = 0;
  while (millis()-last<DELAY) {
    // do other stuff untill is time for a new Record
    delay(DELAY/100);
  }
  last = millis();

  // measurements from all INA devices
  Record* record = new Record(last);
  record->splash(&TERNIMAL);

  // buffer new record
  buffer.unshift(record);

  // dump buffer to CSV file
  if(buffer.isFull()){
    rtc_now();    // new date for filename
    bool newfile = !SD.exists(FILENAME);
    CSV = SD.open(FILENAME, FILE_WRITE);
    if (!CSV) { SD.initErrorHalt(&TERNIMAL); } // errorcode/message to TERNIMAL
    if (newfile){
      buffer.first()->header(&CSV);
    }
    while (!buffer.isEmpty()) {
      buffer.shift()->print(&CSV);
    }
    CSV.close();
  }
}
