/*
Based on
- DisplayReadings example fom INA library
  https://github.com/SV-Zanshin/INA/wiki
*/

#include <Arduino.h>

#include <INA.h>
INA_Class INA;
uint8_t devicesFound = 0;                 // number if INA devices found on I2C bus

#include "EEbuffer.h"                     // circular buffer on EEPROM

#include <SdFat.h>
ArduinoOutStream cout(Serial);            // stream to Serial
SdFat SD;                                 // File system object.
const uint8_t SD_chipSelect = 10;

void setup() {
  Serial.begin(115200);
  devicesFound = INA.begin(1,100000);     // maxBusAmps,microOhmR
  INA.setBusConversion(8500);             // 8.244ms
  INA.setShuntConversion(8500);           // 8.244ms
  INA.setAveraging(128);                  // => ~1 measurement/s
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);  // bus&shunt

  // use the rest of the EEPROM on a circular buffer
  uint8_t bufferStart = devicesFound*sizeof(inaEEPROM); // next EEPROM address after INA devices
  uint8_t chunkSize = (1+devicesFound*2)*sizeof(uint32_t); // timestamp, ch1 mV ... chN uA
  uint8_t maxChunks = EEbuffer.begin(bufferStart, chunkSize);

  cout << F("INA devices on the I2C bus: ") << devicesFound << endl;
  for (uint8_t i=0; i<devicesFound; i++) {
    cout << F("ch") << i << F(": ") << INA.getDeviceName(i) << endl;
  }

  cout << F("Buffering: ") <<
    maxChunks << F(" chunks of ") <<
    chunkSize << F(" bytes\n");

  // header
  cout << F("millis");
  for (uint8_t i=0; i<devicesFound; i++) {
    cout << F(",ch") << i << F(" voltage [mV]")
         << F(",ch") << i << F(" current [uA]");
  }
  cout << endl;
}

void loop() {
  uint32_t timestamp = millis();
  EEbuffer.put(timestamp);
  for (uint8_t i=0; i<devicesFound; i++) {
    EEbuffer.put(INA.getBusMilliVolts(i));
    EEbuffer.put(INA.getBusMicroAmps(i));
  }
  while(EEbuffer.isFull()){
    uint32_t aux=0;
    bool newline = EEbuffer.get(aux);
    if(newline){
      cout << aux << endl;
    }else{
      cout << aux << F(",");
    }
  }
  delay(5000+millis()-timestamp); // wait 5sec in total
}
