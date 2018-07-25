/*
Based on
- DisplayReadings example fom INA library
  https://github.com/SV-Zanshin/INA/wiki
*/

#include <Arduino.h>

#include <INA.h>
INA_Class INA;
uint8_t devicesFound = 0;

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
  for (uint8_t i=0;i<devicesFound;i++) {
    cout << F("ch") << i << F(": ") << INA.getDeviceName(i) << endl;
  }
  
  // header
  cout << F("millis");
  for (uint8_t i=0;i<devicesFound;i++) {
    cout << F(",ch") << i << F(" voltage [mV]")
         << F(",ch") << i << F(" current [uA]");
  }
  cout << endl;
}

void loop() {
  cout << millis();
  for (uint8_t i=0;i<devicesFound;i++) {
    cout << F(",") << INA.getBusMilliVolts(i)
         << F(",") << INA.getBusMicroAmps(i);
  }
  cout << endl;
  delay(5000);
}
