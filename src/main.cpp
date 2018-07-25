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
ArduinoOutStream cout(Serial);            // Serial streams
SdFat SD;                                 // File system object.

void setup() {
  Serial.begin(115200);
  devicesFound = INA.begin(1,100000);     // maxBusAmps,microOhmR
  INA.setBusConversion(8500);             // 8.244ms
  INA.setShuntConversion(8500);           // 8.244ms
  INA.setAveraging(128);                  // 128 samples ==> ~1 measurement/s
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);  // bus&shunt

  cout << F("INA devices on the I2C bus\n");
  for (uint8_t i=0;i<devicesFound;i++) {
    cout << i << F(": ") << 
      INA.getDeviceName(i) << endl;
  }
  cout << F("#,voltage[mV],current[uA]\n");
}

void loop() {
  for (uint8_t i=0;i<devicesFound;i++) {
    cout << i << F(",") << 
      INA.getBusMilliVolts(i) << F(",") << 
      INA.getBusMicroAmps(i) << endl;
  }
  delay(5000);
}
