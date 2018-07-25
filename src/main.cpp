/*
Based on
- DisplayReadings example fom INA library
  https://github.com/SV-Zanshin/INA/wiki
*/

#include <Arduino.h>

#include <INA.h>
INA_Class INA;
uint8_t devicesFound = 0;

#include <EEPROM.h> 

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

  // list channels
  cout << F("INA devices on the I2C bus\n");
  for (uint8_t i=0; i<devicesFound; i++) {
    cout << F("ch") << i << F(": ") << INA.getDeviceName(i) << endl;
  }
  
  // header
  cout << F("millis");
  for (uint8_t i=0; i<devicesFound; i++) {
    cout << F(",ch") << i << F(" voltage [mV]")
         << F(",ch") << i << F(" current [uA]");
  }
  cout << F(",") << endl;
}

void loop() {
  #ifdef __STM32F1__  // emulated EEPROM
  uint16  ee;
  #else
  uint8_t ee;
  #endif

  // buffere results to EEPROM after INA devices
  ee = devicesFound*sizeof(inaEEPROM); // free EEPROM address, after INA devices
  EEPROM.put(ee, (uint32_t)millis());
  for (uint8_t i=0; i<devicesFound; i++) {
    ee += sizeof(uint32_t); EEPROM.put(ee, INA.getBusMilliVolts(i));
    ee += sizeof(uint32_t); EEPROM.put(ee, INA.getBusMicroAmps(i));
  }

  ee = devicesFound*sizeof(inaEEPROM); // free EEPROM address, after INA devices
  uint32_t aux;
  cout << millis();
  for (uint8_t i=0; i<(devicesFound*2+1); i++, ee+=sizeof(uint32_t)) {
    EEPROM.get(ee, aux);
    cout << aux << F(",");
  }
  cout << endl;

  delay(5000);
}
