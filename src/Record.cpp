#include "Record.h"                       // secret sauce ;-)
INA_Class INA;

Record::Record(uint32_t time): time(time) {
  for (uint8_t i=0; i<INA_COUNT; i++) {
    milliVolts[i] = INA.getBusMilliVolts(i);
    microAmps[i] = INA.getBusMicroAmps(i);
  }
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

void Record::splash(Print* out, bool header) {
  if(header){
    out->print(F("rec: ~"));
    out->print(getRecs());
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
}


CircularBuffer<Record*, BUFFER_SIZE> buffer;
