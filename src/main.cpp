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
#ifdef  __AVR_ATmega32U4__
  delay(2000); // wait 2s in leonardo/micro
#endif
  devicesFound = INA.begin(1,100000);     // maxBusAmps,microOhmR
  INA.setBusConversion(8500);             // 8.244ms
  INA.setShuntConversion(8500);           // 8.244ms
  INA.setAveraging(128);                  // samples
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);  // bus&shunt

  Serial.print(F("INA devices on the I2C bus: "));
  Serial.println(devicesFound);
  Serial.println(F("channel,type,voltage[mV],current[uA]"));
}

void loop() {
  for (uint8_t i=0;i<devicesFound;i++) {
    Serial.print(i); Serial.print(F(","));
    Serial.print(INA.getDeviceName(i)); Serial.print(F(","));
    Serial.print(INA.getBusMilliVolts(i)); Serial.print(F(","));
    Serial.println(INA.getBusMicroAmps(i));
  }
  delay(5000);
}
