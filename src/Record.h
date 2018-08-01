#ifndef Record_h
#define Record_h

#include <stdint.h>
#include <Print.h>
#include "config.h"                       // project configuration

#include <INA.h>
extern INA_Class INA;

class Record {
public:
  Record(){};
	Record(uint32_t time);
  ~Record(){};

  inline uint32_t getTime() { return time; }
  inline uint32_t getMilliVolts(uint8_t i) { return (i<INA_COUNT)?milliVolts[i]:0; }
  inline uint32_t getMmicroAmps(uint8_t i) { return (i<INA_COUNT)?microAmps[i]:0; }

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
