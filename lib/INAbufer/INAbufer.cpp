#include "INAbufer.h"                     // secret sauce ;-)
CircularBuffer<Record*, BUFFER_LEN> buffer;
static char linebuffer[16];               // long enough for 1 line

#include <INA.h>
INA_Class INA;
static const uint8_t  maxBusAmps = 1;
static const uint32_t microOhmR = 100000;

Record::Record(uint32_t time): time(time) {
  for (uint8_t i=0; i<INA_COUNT; i++) {
    milliVolts[i] = INA.getBusMilliVolts(i);
    microAmps[i] = INA.getBusMicroAmps(i);
  }
}

char *Record::getRunTime(uint32_t s){
  uint8_t d, h, m;
  d = s/86400; s %= 86400;  // 86400 secs on a day
  h = s/ 3600; s %=  3600;  //  3600 secs on a hour
  m = s/   60; s %=    60;  //    60 secs on a minute
  sprintf(linebuffer, "%02u:%02u:%02u:%02u", d, h, m, (uint8_t)s);
  return linebuffer;
}

uint8_t Record::init(){
  uint8_t INAfound = INA.begin(maxBusAmps, microOhmR);
  INA.setBusConversion(INA_CONVTIME);     // see config.h for value
  INA.setShuntConversion(INA_CONVTIME);   // see config.h for value
  INA.setAveraging(INA_SAMPLES);          // see config.h for value
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);  // bus&shunt
  return INAfound;
}

uint8_t Record::init(Print* out){
  uint8_t INAfound = init();
  while (INAfound != INA_COUNT) {  
    out->print(F("ERROR: INA devices expected "));
    out->print(INA_COUNT);
    out->print(F(", found "));
    out->println(INAfound);
    delay(1000);
    INAfound = init(); // try again
  }

  for (uint8_t i=0; i<INAfound; i++) {
    out->print(F("INA"));
    out->print(i);
    out->print(F(": "));
    out->println(INA.getDeviceName(i));
  }

  out->print(F("Buffer "));
  out->print(RECORD_SIZE);
  out->print(F("Bx"));
  out->print(BUFFER_LEN);
  out->println("rec");

  return INAfound;
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

void Record::splash(Print* out, uint8_t width, bool header) {
  switch (width) {
    case 16 ... 255:  // wide screen. eg 128x64 or 128x32
      //                            0123456789ABCDEF
      //                           "1: 23.000  1.000"
      if (header) { out->print(F("#   V [V]  I [A]\n")); }
      for (uint8_t i=0; i<INA_COUNT; i++) {
        out->print(i);out->print(F(":"));
        out->print(dtostrf(getVolts(i),7,3,linebuffer));
        out->print(dtostrf(getAmps(i) ,7,3,linebuffer));
        out->print(F("\n"));
      }
    break;
    case 4 ... 15:  // narrow screem, eg 84X48 or 64X48
      //              012345678
      //             "V1 23.000"
      //             "A1  1.000"
      header &= width > 6;
      for (uint8_t i=0; i<INA_COUNT; i++) {
        if (header) { out->print(F("V")); }
        out->print(i);
        out->print(dtostrf(getVolts(i),7,3,linebuffer));
        out->print(F("\n"));
        if (header) { out->print(F("A")); }
        out->print(i);
        out->print(dtostrf(getAmps(i) ,7,3,linebuffer));
        out->print(F("\n"));
      }
    break;
  }
}
