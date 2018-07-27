#include "logger.h"

#include <INA.h>
INA_Class INA;
uint8_t devicesFound = 0;

#include <EEPROM.h> 
uint8_t eeStart, ee;

#ifdef __STM32F1__  // emulated EEPROM
#define EElen EEPROM.maxcount
void EEget(uint8_t address, uint32_t data);
void EEput(uint8_t address, uint32_t data);
#else
#define EElen EEPROM.length
#define EEget EEPROM.get
#define EEput EEPROM.put
#endif

void LOGbegin(){
  Serial.begin(115200);
  devicesFound = INA.begin(1,100000);     // maxBusAmps,microOhmR
  INA.setBusConversion(8500);             // 8.244ms
  INA.setShuntConversion(8500);           // 8.244ms
  INA.setAveraging(128);                  // => ~1 measurement/s
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);  // bus&shunt
  eeStart = devicesFound*sizeof(inaEEPROM); // next EEPROM address after INA devices
  ee = eeStart;  
}

void LOGheader(ArduinoOutStream *cout){
  // list channels
  (*cout) << F("INA devices on the I2C bus\n");
  for (uint8_t i=0; i<devicesFound; i++) {
    (*cout) << F("ch") << i << F(": ") << INA.getDeviceName(i) << endl;
  }

  // header
  (*cout) << F("millis");
  for (uint8_t i=0; i<devicesFound; i++) {
    (*cout) << F(",ch") << i << F(" voltage [mV]")
         << F(",ch") << i << F(" current [uA]");
  }
  (*cout) << endl;
}

bool LOGsave(){
  uint8_t chunkSize=(devicesFound*2+1)*sizeof(uint32_t);
  // is there new set of measurement?
  if((ee+chunkSize)>EElen()){
    return false;
  }
  // buffere results to EEPROM after INA devices
  EEput(ee, (uint32_t)millis()); ee += sizeof(uint32_t);
  for (uint8_t i=0; i<devicesFound; i++) {
    EEput(ee, INA.getBusMilliVolts(i)); ee += sizeof(uint32_t);
    EEput(ee, INA.getBusMicroAmps(i));  ee += sizeof(uint32_t); 
  }
  return true;
}

void LOGdump(ArduinoOutStream *cout){
  uint8_t chunkSize=(devicesFound*2+1)*sizeof(uint32_t);
  uint32_t aux=0;
  for (uint8_t e=eeStart; e<ee; e+=sizeof(uint32_t)) {
    EEget(e, aux);
    if((e-eeStart+sizeof(uint32_t))%chunkSize == 0){ // last in chunk
      (*cout) << aux << endl;
    } else{
      (*cout) << aux << F(",");
    }
  }
  ee = eeStart;
}

#ifdef __STM32F1__  // emulated EEPROM
// EEPROM.get for uint32_t
void EEget(uint8_t address, uint32_t data){
  uint16 e = address;
  uint16 *ptr = (uint16*) &data;
  for(uint8_t n = sizeof(uint32_t); n ;--n) {
    EEPROM.read(e++, ptr++);
  }
}
// EEPROM.put for uint32_t
void EEput(uint8_t address, uint32_t data){
  uint16 e = address;
  uint16 *ptr = (uint16*) &data;
  for(uint8_t n = sizeof(uint32_t); n ;--n) {
    EEPROM.update(e++, *ptr++);
  }
}
#endif
