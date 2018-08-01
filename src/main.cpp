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

#include <INA.h>
INA_Class INA;

#include <SdFat.h>
SdFat SD;                                 // File system object.
File CSV;                                 // File object for filename
#ifdef HAST_RTC
#include "RTCutil.h"
char filename[16];                        // 'YYMMDD.csv'
#else
#define filename "INA.csv"
#endif

#include <CircularBuffer.h>
class Record {
private:
  uint32_t time;
  uint32_t milliVolts[INA_COUNT];
  uint32_t microAmps[INA_COUNT];

public:
  Record(){};
  ~Record(){};
	Record(uint32_t time): time(time) {
    for (uint8_t i=0; i<INA_COUNT; i++) {
      milliVolts[i] = INA.getBusMilliVolts(i);
      microAmps[i] = INA.getBusMicroAmps(i);
    }
	}

  void header(Print* out) {
    out->print(F("millis"));
    for (uint8_t i=0; i<INA_COUNT; i++) {
      out->print(F(",ch"));out->print(i);out->print(F(" voltage [mV]"));
      out->print(F(",ch"));out->print(i);out->print(F(" current [uA]"));
    }
    out->println();
  }

  void print(Print* out) {
		out->print(time);
    for (uint8_t i=0; i<INA_COUNT; i++) {
      out->print(F(","));out->print(milliVolts[i]);
      out->print(F(","));out->print(microAmps[i]);
    }
    out->println();
	}
};

CircularBuffer<Record*, BUFFER_SIZE> buffer;

void setup() {
  Serial.begin(57600);                    // for ATmega328p 3.3V 8Mhz
  while(!Serial){ SysCall::yield(); }     // Wait for USB Serial

  // set RTClock
#ifdef HAST_RTC
  if(rtc_now()<BUILD_TIME){
    rtc_now(BUILD_TIME);
    Serial.print(F("Set RTC to built time: "));
    Serial.println(BUILD_TIME);
  }
#endif

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

#ifdef HAST_RTC
  char datestr[12];
  Serial.print(F("Logging start: "));
  Serial.print(rtc_fmt(datestr, 'D'));    // long date
  Serial.print(F(" "));
  Serial.print(rtc_fmt(datestr, 'T'));    // long time
  Serial.print(F("UTC @"));
  Serial.println(rtc_now());
#endif

  Serial.print(F("Buffering "));
  Serial.print(BUFFER_SIZE);
  Serial.print(F(" records of "));
  Serial.print(RECORD_SIZE*sizeof(uint32_t));
  Serial.print(" bytes before writing to SD:");
#ifdef HAST_RTC
  Serial.println(rtc_fmt(filename, 'C')); // YYMMDD.csv
#else
  Serial.println(filename);
#endif
}

void loop() {

  // buffer new data chunk
  uint32_t timestamp = millis();
  Record* record = new Record(timestamp);
  buffer.unshift(record);

  // dump buffer to CSV file
  if(buffer.isFull()){
    bool newfile = !SD.exists(filename);
    CSV = SD.open(filename, FILE_WRITE);
    if (!CSV) { SD.initErrorHalt(); }     // errorcode/message to Serial
    if (newfile){
      buffer.first()->header(&CSV);
    }
    while (!buffer.isEmpty()) {
      buffer.shift()->print(&CSV);
    }
    CSV.close();
#ifdef HAST_RTC
    // update filename for next set of Records
    rtc_now();
    rtc_fmt(filename, 'C'); // file.csv
#endif
  }

  // wait until next measurement time
  delay(millis()-timestamp+DELAY);
}
