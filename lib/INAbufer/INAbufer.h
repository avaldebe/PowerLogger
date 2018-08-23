#ifndef INAbufer_h
#define INAbufer_h

#include <stdint.h>
#include <stdio.h>
#include <Print.h>
#include "config.h" // project configuration

class Record {
public:
  Record(){};
	Record(uint32_t time);
  ~Record(){};

  inline uint32_t getTime() { return time; }
  inline uint32_t getMilliVolts(uint8_t i) { return (i<INA_COUNT)?milliVolts[i]:0; }
  inline uint32_t getMicroAmps(uint8_t i) { return (i<INA_COUNT)?microAmps[i]:0; }
  inline float getVolts(uint8_t i) { return (float)getMilliVolts(i)/1000; }
  inline float getAmps(uint8_t i) { return (float)getMicroAmps(i)/1000000; }

  static char *getRunTime(uint32_t secs);
  inline char *getRunTime(){ return getRunTime(time/1000); }

  static uint8_t init();
  static uint8_t init(Print* out);
  void header(Print* out);
  void print(Print* out);
  void splash(Print* out, uint8_t width=16, bool header=true);

protected:
  uint32_t time;
  uint32_t milliVolts[INA_COUNT];
  uint32_t microAmps[INA_COUNT];
};

#include <CircularBuffer.h>
extern CircularBuffer<Record*, BUFFER_SIZE> buffer;

#endif
