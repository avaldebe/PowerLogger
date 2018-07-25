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
#ifdef __STM32F1__  // emulated EEPROM
#define EEPROM_len EEPROM.maxcount
void EEPROM_get(uint8_t address, uint32_t data);
void EEPROM_put(uint8_t address, uint32_t data);
#else
#define EEPROM_len EEPROM.length
#define EEPROM_get EEPROM.get
#define EEPROM_put EEPROM.put
#endif
uint8_t eeStart, ee;
bool EEbuffer();   // buffer measurement in EEPROM
void EEdump();     // dump EEPROM buffered measuremnts

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
  eeStart = devicesFound*sizeof(inaEEPROM); // next EEPROM address after INA devices
  ee = eeStart;

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
  cout << endl;
}

void loop() {
  if(EEbuffer()){  // new set of measurements stored on EEPROM
    delay(5000);    
  } else {         // not enoug space for a new set of measurenets
    EEdump();      // empty EEPROM buffer
  }
}

bool EEbuffer(){
  uint8_t chunkSize=(devicesFound*2+1)*sizeof(uint32_t);
  // is there new set of measurement?
  if((ee+chunkSize)>EEPROM_len()){
    return false;
  }
  // buffere results to EEPROM after INA devices
  EEPROM_put(ee, (uint32_t)millis()); ee += sizeof(uint32_t);
  for (uint8_t i=0; i<devicesFound; i++) {
    EEPROM_put(ee, INA.getBusMilliVolts(i)); ee += sizeof(uint32_t);
    EEPROM_put(ee, INA.getBusMicroAmps(i));  ee += sizeof(uint32_t); 
  }
  return true;
}

void EEdump(){
  uint8_t chunkSize=(devicesFound*2+1)*sizeof(uint32_t);
  uint32_t aux=0;
  for (uint8_t e=eeStart; e<ee; e+=sizeof(uint32_t)) {
    EEPROM_get(e, aux);
    if((e-eeStart+sizeof(uint32_t))%chunkSize == 0){ // last in chunk
      cout << aux << endl;
    } else{
      cout << aux << F(",");
    }
  }
  ee = eeStart;
}

#ifdef __STM32F1__  // emulated EEPROM
// EEPROM.get for uint32_t
void EEPROM_get(uint8_t address, uint32_t data){
  uint16 e = address;
  uint16 *ptr = (uint16*) &data;
  for(uint8_t n = sizeof(uint32_t); n ;--n) {
    EEPROM.read(e++, ptr++);
  }
}
// EEPROM.put for uint32_t
void EEPROM_put(uint8_t address, uint32_t data){
  uint16 e = address;
  uint16 *ptr = (uint16*) &data;
  for(uint8_t n = sizeof(uint32_t); n ;--n) {
    EEPROM.update(e++, *ptr++);
  }
}
#endif
