#ifndef Record_h
#define Record_h

#include <Print.h>
#include <stdint.h>
#include "config.h"                       // project configuration

#include <INA.h>
extern INA_Class INA;

class Record {
public:
  Record(){};
	Record(uint32_t time);
  ~Record(){};
  void header(Print* out);
  void print(Print* out);

protected:
  uint32_t time;
  uint32_t milliVolts[INA_COUNT];
  uint32_t microAmps[INA_COUNT];
};

#include <CircularBuffer.h>
extern CircularBuffer<Record*, BUFFER_SIZE> buffer;

#endif
