#ifndef INAbufer_h
#define INAbufer_h

#include <stdint.h>
#include <stdio.h>
#include <Print.h>
#include "../../src/config.h" // project configuration

class Record {
public:
  Record(){};
	Record(uint32_t time);
  ~Record(){ delete [] milliVolts; delete [] microAmps; };

  inline uint32_t getTime() { return time; }
  inline uint32_t getMilliVolts(uint8_t i) { return (i<ina_count)?milliVolts[i]:0; }
  inline uint32_t getMicroAmps(uint8_t i) { return (i<ina_count)?microAmps[i]:0; }
  inline float getVolts(uint8_t i) { return (float)getMilliVolts(i)/1000; }
  inline float getAmps(uint8_t i) { return (float)getMicroAmps(i)/1000000; }

  static char *getRunTime(uint32_t secs);
  inline char *getRunTime(){ return getRunTime(time/1000); }
  
  static uint8_t init(uint8_t maxBusAmps=1, uint32_t microOhmR=100000);
  static uint8_t init(Print* out);
  void header(Print* out);
  void print(Print* out);
  void splash(Print* out, uint8_t width=16, bool header=true);

  // size of 1 record from n channels
  #define RECORD_SIZE(n)  ((1+2*n)*sizeof(uint32_t))
  static inline uint8_t size(){ return RECORD_SIZE(ina_count); }

  // max records(n channels) in buffer
  #define BUFFER_MAX(n)   (BUFFER_SIZE/RECORD_SIZE(n))
  static inline uint8_t max_len(){ return BUFFER_MAX(ina_count); }

protected:
  static uint8_t ina_count;
  uint32_t time;
  uint32_t *milliVolts;
  uint32_t *microAmps;
};

#include <CircularBuffer.h>
#define BUFFER_LEN    BUFFER_MAX(2)
extern CircularBuffer<Record*, BUFFER_LEN> buffer;

inline bool buffer_full() { return buffer.isFull() || buffer.size()>=Record::max_len();}

#endif
