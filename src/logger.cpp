#include "logger.h"

#include <INA.h>
static INA_Class INA;

#include <EEPROM.h> 
#ifdef __STM32F1__  // emulated EEPROM
#define EElen EEPROM.maxcount
template< typename T > static void EEget(uint8_t address, T data){ // EEPROM.get
  uint16 e = address;
  uint16 *ptr = (uint16*) &data;
  for(uint8_t n = sizeof(T); n ;--n) {
    EEPROM.read(e++, ptr++);
  }
}
template< typename T > static void EEput(uint8_t address, T data){ // EEPROM.put
  uint16 e = address;
  uint16 *ptr = (uint16*) &data;
  for(uint8_t n = sizeof(T); n ;--n) {
    EEPROM.update(e++, *ptr++);
  }
}
#else
#define EElen EEPROM.length
#define EEget EEPROM.get
#define EEput EEPROM.put
#endif

void LOGGClass::begin(){
  devicesFound = INA.begin(1,100000);     // maxBusAmps,microOhmR
  INA.setBusConversion(8500);             // 8.244ms
  INA.setShuntConversion(8500);           // 8.244ms
  INA.setAveraging(128);                  // => ~1 measurement/s
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);  // bus&shunt

  bufferStart = devicesFound*sizeof(inaEEPROM); // next EEPROM address after INA devices
  chunkNext = 0;                      // 1st chunk @ bufferStart
  
  // 1 chunk = timestamp, ch1 millivolts, ... ,chN microamp
  chunkSize = (1+devicesFound*2)*sizeof(uint32_t);
  chunkMaxCount = (EElen()-bufferStart)/chunkSize;
}

void LOGGClass::header(ArduinoOutStream *cout){
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

bool LOGGClass::save(){
  if(bufferFull()){   // is there space for a set of measurement?
    chunkNext=0;      // overwrite 1st measurement
  }
  // buffere results to EEPROM after INA devices
  uint8_t ee=bufferNext();
  EEput(ee, (uint32_t)millis()); ee += sizeof(uint32_t);
  for (uint8_t i=0; i<devicesFound; i++) {
    EEput(ee, INA.getBusMilliVolts(i)); ee += sizeof(uint32_t);
    EEput(ee, INA.getBusMicroAmps(i));  ee += sizeof(uint32_t); 
  }
  chunkNext++;
  return bufferFull();
}

uint32_t LOGGClass::read(uint8_t chunk, uint8_t idx){ //reasd single value within saved chunk
  uint32_t aux=0;
  EEget(bufferLast(chunk, idx), aux);
  return aux;
}

void LOGGClass::dump(ArduinoOutStream *cout){
  for (uint8_t n=0; n<chunkNext; n++) {
    for (uint8_t i=0; i<devicesFound*2; i++) {
      (*cout) << read(n,i) << F(",");
    }
    (*cout) << read(n,devicesFound*2) << endl;
  }
  chunkNext = 0;
}
