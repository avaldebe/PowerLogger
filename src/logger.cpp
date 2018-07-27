#include "logger.h"

#include <INA.h>
INA_Class INA;
uint8_t devicesFound = 0;

#include <EEPROM.h> 
#ifdef __STM32F1__  // emulated EEPROM
#define EElen EEPROM.maxcount
template< typename T > static void EEget(uint8_t address, T data); // EEPROM.get
template< typename T > static void EEput(uint8_t address, T data); // EEPROM.put
#else
#define EElen EEPROM.length
#define EEget EEPROM.get
#define EEput EEPROM.put
#endif

// buffer statasets to EEPROM
uint8_t bufferStart, chunkSize, chunkMaxCount, chunkNext;
inline bool    bufferFull(){ return chunkNext>chunkMaxCount; }
inline uint8_t bufferNext(){ return bufferStart+chunkNext*chunkSize; }
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
inline uint8_t bufferLast(uint8_t chunk, uint8_t idx=0){ 
  return bufferStart
    +MIN(chunk,chunkMaxCount-1)*chunkSize
    +MIN(idx,devicesFound*2)*sizeof(uint8_t); 
}
inline uint8_t bufferLast(){ return bufferLast(chunkNext-1); }

void LOGbegin(){
  Serial.begin(115200);
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

uint32_t LOGread(uint8_t chunk, uint8_t idx){ // index within last chunk
  uint32_t aux=0;
  EEget(bufferLast(chunk, idx), aux);
  return aux;
}

void LOGdump(ArduinoOutStream *cout){
  for (uint8_t n=0; n<chunkNext; n++) {
    for (uint8_t i=0; i<devicesFound*2; i++) {
      (*cout) << LOGread(n,i) << F(",");
    }
    (*cout) << LOGread(n,devicesFound*2) << endl;
  }
  chunkNext = 0;
}

#ifdef __STM32F1__  // emulated EEPROM
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
#endif
