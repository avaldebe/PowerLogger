#include "Record.h"                       // secret sauce ;-)
CircularBuffer<Record*, BUFFER_SIZE> buffer;

#include <INA.h>
static INA_Class INA;

Record::Record(uint32_t time): time(time) {
  for (uint8_t i=0; i<INA_COUNT; i++) {
    milliVolts[i] = INA.getBusMilliVolts(i);
    microAmps[i] = INA.getBusMicroAmps(i);
  }
}

char *Record::getRunTime(uint32_t s){
  uint8_t d, h, m;
  static char str[16];      // just enough for "00:00:00:00"
  d = s/86400; s %= 86400;  // 86400 secs on a day
  h = s/ 3600; s %=  3600;  //  3600 secs on a hour
  m = s/   60; s %=    60;  //    60 secs on a minute
  sprintf(str, "%02u:%02u:%02u:%02u", d, h, m, (uint8_t)s);
  return str;
}

void Record::init(Print* out, const char *filename){
  uint8_t INAfound = INA.begin(1,100000); // maxBusAmps,microOhmR
  while (INAfound != INA_COUNT) {
    out->print(F("ERROR: INA devices expected "));
    out->print(INA_COUNT);
    out->print(F(", found "));
    out->println(INAfound);
    delay(100);
  }
  INA.setBusConversion(INA_CONVTIME);     // see config.h for value
  INA.setShuntConversion(INA_CONVTIME);   // see config.h for value
  INA.setAveraging(INA_SAMPLES);          // see config.h for value
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);  // bus&shunt

  out->print(F("INA devices on the I2C bus: "));
  out->println(INA_COUNT);
  for (uint8_t i=0; i<INA_COUNT; i++) {
    out->print(F("ch"));
    out->print(i);
    out->println(INA.getDeviceName(i));
  }

  out->print(F("Buffering "));
  out->print(BUFFER_SIZE);
  out->print(F(" records of "));
  out->print(RECORD_SIZE*sizeof(uint32_t));
  out->print(" bytes before writing to SD:");
  out->println(filename);
}

void Record::header(Print* out) {
  out->print(F("millis"));
  for (uint8_t i=0; i<INA_COUNT; i++) {
    out->print(F(",ch"));out->print(i);out->print(F(" voltage [mV]"));
    out->print(F(",ch"));out->print(i);out->print(F(" current [uA]"));
  }
  out->println();
}

void Record::print(Print* out) {
	out->print(time);
  for (uint8_t i=0; i<INA_COUNT; i++) {
    out->print(F(","));out->print(milliVolts[i]);
    out->print(F(","));out->print(microAmps[i]);
  }
  out->println();
}

void Record::splash(Print* out, bool header, bool footer) {
  if(header){
    //           "1  23.000  1.000"
    out->print(F("#   V [V]  I [A]"));
  }
  for (uint8_t i=0; i<INA_COUNT; i++) {
    out->print(i);
    out->print(F(": "));
    out->print((milliVolts[i]<10000)?F("   "):F("  "));
    out->print(getVolts(i),3);
    out->print(F("  "));
    out->println(getAmps(i),3);
  }
  if(footer){
    out->println(getRunTime());
  }
}
