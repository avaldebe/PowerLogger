/*
Circular EEPROM buffer
- quick&dirty support for STM32F1 emulated EEPROM
- use a section of the buffer: from bufferStart to bufferStart+maxCount
- the buffer is subivided in data chunks of chunkSize bytes
*/

#ifndef EEbuffer_h                                                          // Guard code definition            //
#define EEbuffer_h                                                        // Define the name inside guard code//

#include <EEPROM.h>
#ifdef __STM32F1__  // emulated EEPROM
#define EElen EEPROM.maxcount
template< typename T > static void EEget(uint8_t address, T &data){ // EEPROM.get
  uint16 e = address;
  uint16 *ptr = (uint16*) &data;
  for(uint8_t n = sizeof(T); n ;--n) {
    EEPROM.read(e++, ptr++);
  }
}
template< typename T > static void EEput(uint8_t address, const T &data){ // EEPROM.put
  uint16 e = address;
  uint16 *ptr = (uint16*) &data;
  for(uint8_t n = sizeof(T); n ;--n) {
    EEPROM.update(e++, *ptr++);
  }
}
#else
#define EElen EEPROM.length
#define EEget EEPROM.get
#define EEput EEPROM.put
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

struct EEBClass{
private:
  uint8_t bufferStart;  // 1st addrss of the EEPROM buffer
  uint8_t chunkSize;    // EEPROM cells needed for one dataset
  uint8_t maxCount;     // number of cells on EEPROM buffer
  uint8_t idxPut;       // 0 .. maxCount-1
  uint8_t idxGet;       // 0 .. idxPut-1

public:
  uint8_t begin(uint8_t start=0x00, uint8_t size=sizeof(uint32_t), uint8_t length=0xFF){
    bufferStart = start;                          // beggining of the buffer
    chunkSize = size;                             // size of 1 data chunk
    maxCount = MIN(length, EElen()-bufferStart);  // maximum available lenght
    maxCount -= maxCount%chunkSize;               // only full data chunks
    idxPut = 0;                                   // 0 .. maxCount-1
    idxGet = 0;                                   // 0 .. idxPut-1
    return maxCount/chunkSize;                    // max number of chunks
  }

  inline bool isFull(){ return idxPut >= maxCount;  }
  inline bool endChunk(){ return idxGet%chunkSize == 0;  }
  template< typename T > bool put(const T &data){
    if(isFull()){
      idxPut = 0;                     // 0 .. maxCount-1
    }
    EEput(bufferStart+idxPut, data);  // save to EEPROM
    idxPut += sizeof(T);              // advance address for next put
    return isFull();                  // end of data buffer
  }
  template< typename T > bool get(T &data){
    if(idxGet >= idxPut){
      idxGet = 0;                     // 0 .. idxPut-1
    }
    EEget(bufferStart+idxGet, data);  // read from EEPROM
    idxGet += sizeof(T);              // advance address for next get
    if (idxGet >= idxPut) {           // read end of buffer
      idxPut = 0;                     // reset idxPut to beggining of buffer
    }
    return endChunk();                 // end of data chunk
  }
  template< typename T > bool &last(T &data){
    static uint8_t n=0;               // address within the last data chunk
    uint8_t idx=idxPut-1;             // last used address
    idx -= idx%chunkSize;             // beggining of last data chunk
    EEget(bufferStart+idx+n, data);   // read from EEPROM
    n += sizeof(T);                   // next EEPROM address
    n %= chunkSize;                   // within the data chunk
    return n == 0;                    // end of data chunk
  }
};

static EEBClass EEbuffer;
#endif
