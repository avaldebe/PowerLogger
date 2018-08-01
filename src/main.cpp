/*
Based on
- DisplayReadings example fom INA library
  https://github.com/SV-Zanshin/INA
- QuickStart example form the SdFat library
  https://github.com/greiman/SdFat
- Object example on the CircularBuffer library
  https://github.com/rlogiacco/CircularBuffer
*/

#include <Arduino.h>
#include "config.h"                       // project configuration
#include "Record.h"                       // secret sauce ;-)

#include <SdFat.h>
SdFat SD;                                 // File system object.
File CSV;                                 // File object for filename

#include "RTCutil.h"
#ifdef HAST_RTC
#define FILENAME rtc_fmt('C')             // 'YYMMDD.csv'
#else
#define FILENAME "INA.csv"
#endif

void setup() {
  Serial.begin(57600);                    // for ATmega328p 3.3V 8Mhz
  while(!Serial){ SysCall::yield(); }     // Wait for USB Serial
  rtc_begin(&Serial);                     // update RTC if needed

  if (!SD.begin(chipSelect, SPI_SPEED)) {
    SD.initErrorHalt();                   // errorcode/message to Serial
  }

  uint8_t INAfound = INA.begin(1,100000); // maxBusAmps,microOhmR
  while(INAfound != INA_COUNT){
    Serial.print(F("ERROR: INA devices expected "));
    Serial.print(INA_COUNT);
    Serial.print(F(", found "));
    Serial.println(INAfound);
    delay(DELAY);
  }
  INA.setBusConversion(INA_CONVTIME);     // see config.h for value
  INA.setShuntConversion(INA_CONVTIME);   // see config.h for value
  INA.setAveraging(INA_SEMPLES);          // see config.h for value
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);  // bus&shunt

  Serial.print(F("INA devices on the I2C bus: "));
  Serial.println(INA_COUNT);
  for (uint8_t i=0; i<INA_COUNT; i++) {
    Serial.print(F("ch"));
    Serial.print(i);
    Serial.println(INA.getDeviceName(i));
  }

  Serial.print(F("Buffering "));
  Serial.print(BUFFER_SIZE);
  Serial.print(F(" records of "));
  Serial.print(RECORD_SIZE*sizeof(uint32_t));
  Serial.print(" bytes before writing to SD:");
  Serial.println(FILENAME);
}

void loop() {

  // buffer new data chunk
  Record* record = new Record(millis());
  buffer.unshift(record);

  // dump buffer to CSV file
  if(buffer.isFull()){
    rtc_now();    // new date for filename
    bool newfile = !SD.exists(FILENAME);
    CSV = SD.open(FILENAME, FILE_WRITE);
    if (!CSV) { SD.initErrorHalt(); }     // errorcode/message to Serial
    if (newfile){
      buffer.first()->header(&CSV);
    }
    while (!buffer.isEmpty()) {
      buffer.shift()->print(&CSV);
    }
    CSV.close();
  }

  // wait until next measurement time
  delay(millis()-record->getTime()+DELAY);
}
