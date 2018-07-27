#ifndef LOGG_h                                                          // Guard code definition            //
#define LOGG_h                                                        // Define the name inside guard code//

#include <SdFat.h>

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

class LOGGClass{
  public:
    LOGGClass(){};                        // constructor
    ~LOGGClass(){};                       // destructor
    void begin();                         // INA.begin ...
    void header(ArduinoOutStream *cout);  // stream header
    bool save();                          // buffer measurement in EEPROM
    void dump(ArduinoOutStream *cout);    // stream buffered measuremnts

  private:
    uint8_t devicesFound = 0;             // number if INA devices found on I2C bus
    uint8_t bufferStart = 0;              // next EEPROM address after INA devices
    uint8_t chunkSize = sizeof(uint32_t); // EEPROM cells needed for one dataset (timestamp, ...)
    uint8_t chunkMaxCount = 1;            //
    uint8_t chunkNext = 0;                // 0 .. chunkMaxCount-1
    
    inline bool    bufferFull(){ return chunkNext>chunkMaxCount; }
    inline uint8_t bufferNext(){ return bufferStart+chunkNext*chunkSize; }
    inline uint8_t bufferLast(uint8_t chunk, uint8_t idx=0){ 
      return bufferStart
        +MIN(chunk,chunkMaxCount-1)*chunkSize
        +MIN(idx,devicesFound*2)*sizeof(uint8_t); 
    }
    inline uint8_t bufferLast(){ return bufferLast(chunkNext-1); }
    
    uint32_t read(uint8_t chunk, uint8_t idx);  // get single value within saved chunk
};

static LOGGClass LOGG;
#endif
