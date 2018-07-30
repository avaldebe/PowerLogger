/*
Based on
- DisplayReadings example fom INA library
  https://github.com/SV-Zanshin/INA
- QuickStart example form the SdFat library
  https://github.com/greiman/SdFat
*/

#include <Arduino.h>

#include <INA.h>
INA_Class INA;
uint8_t devicesFound = 0;                 // number if INA devices found on I2C bus

#include <SdFat.h>
SdFat SD;                                 // File system object.
File CSV;                                 // see config.h for FILENAME
ArduinoOutStream cout(Serial);            // stream to Serial
ArduinoOutStream csv(CSV);                // stream to CSV file

#include "EEbuffer.h"                     // circular buffer on EEPROM
#include "config.h"                       // project configuration

void setup() {
  Serial.begin(57600);                    // for ATmega328p 3.3V 8Mhz
  while(!Serial){ SysCall::yield(); }     // Wait for USB Serial
  if (!SD.begin(chipSelect, SPI_SPEED)) {
    SD.initErrorHalt();                   // errorcode/message to Serial
  }

  devicesFound = INA.begin(1,100000);     // maxBusAmps,microOhmR
  INA.setBusConversion(INA_CONVTIME);     // see config.h for value
  INA.setShuntConversion(INA_CONVTIME);   // see config.h for value
  INA.setAveraging(INA_SEMPLES);          // see config.h for value
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);  // bus&shunt

  // use the rest of the EEPROM on a circular buffer
  uint8_t bufferStart, chunkSize, maxChunks;
  bufferStart = devicesFound*sizeof(inaEEPROM);     // next EEPROM address after INA devices
  chunkSize = (1+devicesFound*2)*sizeof(uint32_t);  // timestamp, ch1 mV, ch1 uA, ...
  maxChunks = EEbuffer.begin(bufferStart, chunkSize);

  cout << F("INA devices on the I2C bus: ") << devicesFound << endl;
  for (uint8_t i=0; i<devicesFound; i++) {
    cout << F("ch") << i << F(": ") << INA.getDeviceName(i) << endl;
  }

  cout << F("Buffering ") <<
    maxChunks << F(" chunks of ") <<
    chunkSize << F(" bytes before writing to SD:") << FILENAME << endl;

  // write header to CSV file
  CSV = SD.open(FILENAME, FILE_WRITE);
  if (!CSV) { SD.initErrorHalt(); }       // errorcode/message to Serial
  csv << F("millis");
  for (uint8_t i=0; i<devicesFound; i++) {
    csv << F(",ch") << i << F(" voltage [mV]")
        << F(",ch") << i << F(" current [uA]");
  }
  csv << endl;
  CSV.close();
}

void loop() {
  
  // buffer new data chunk
  uint32_t timestamp = millis();
  EEbuffer.put(timestamp);
  for (uint8_t i=0; i<devicesFound; i++) {
    EEbuffer.put(INA.getBusMilliVolts(i));
    EEbuffer.put(INA.getBusMicroAmps(i));
  }

  // dump buffer to CSV file
  uint32_t aux=0;
  if(EEbuffer.isFull()){
    CSV = SD.open(FILENAME, FILE_WRITE);
    if (!CSV) { SD.initErrorHalt(); }     // errorcode/message to Serial
    while(EEbuffer.isFull()){
      bool newline = EEbuffer.get(aux);
      if(newline){
        csv << aux << endl;
      }else{
        csv << aux << F(",");
      }
    }
      CSV.close();
  }

  // wait until next measurement time
  delay(millis()-timestamp+DELAY);
}
