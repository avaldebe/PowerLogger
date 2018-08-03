#ifndef config_h
#define config_h
#include <stdint.h>

/* SD CARD:
Reduced SPI speed for breadboards:
SD_SCK_MHZ(4) will select the highest speed supported by the board under 4 MHz.
Change SPI_SPEED to SD_SCK_MHZ(50) for best performance. */
#define SPI_SPEED SD_SCK_MHZ(4)
const uint8_t chipSelect = 10;

// Measurament frequency
#define INA_CONVTIME  8600             // maximum conversion time [us]:
                                       // 8.510ms on INA219, 8.244ms elsewhere
#define SAMPLES(frec,conv)  ((uint16_t)frec*1000/(conv*2))
#define INA_SAMPLES   SAMPLES(FREQUENCY,INA_CONVTIME) // fit FREQUENCY
#ifndef FREQUENCY                      // ms between measurements
#define FREQUENCY     1000             // default
#endif

// Measurement array and buffer size
#ifndef INA_COUNT                      // Number if INA devices
#define INA_COUNT        3             // default
#endif
#define RECORD_SIZE    (1+2*INA_COUNT)
#ifndef BUFFER_SIZE                    // Number of records in buffer
#define BUFFER_SIZE    128/sizeof(uint32_t)/RECORD_SIZE // 128 bytes
#endif

#endif
