/*
Based on
- DisplayReadings example fom INA library
  https://github.com/SV-Zanshin/INA/wiki
*/

#include <Arduino.h>

#include <INA.h>
INA_Class INA;
uint8_t devicesFound = 0;

void setup() {
  Serial.begin(115200);
  devicesFound = INA.begin(1,100000);     // maxBusAmps,microOhmR
  INA.setBusConversion(8500);             // 8.244ms
  INA.setShuntConversion(8500);           // 8.244ms
  INA.setAveraging(128);                  // 128 samples ==> ~1 measurement/s
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);  // bus&shunt

  Serial.print(F("INA devices on the I2C bus: "));
  for (uint8_t i=0;i<devicesFound;i++) {
    Serial.print(i); Serial.print(F(": "));
    Serial.println(INA.getDeviceName(i));
  }

  Serial.println(devicesFound);
  Serial.println(F("#,voltage[mV],current[uA]"));
}

void loop() {
  for (uint8_t i=0;i<devicesFound;i++) {
    Serial.print(i); Serial.print(F(","));
    Serial.print(INA.getBusMilliVolts(i)); Serial.print(F(","));
    Serial.println(INA.getBusMicroAmps(i));
  }
  delay(5000);
}
