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
#define INA_CONVTIME  8500             // Maximum conversion time 8.244ms
#define INA_SEMPLES    128             // Samples in average => ~1 measurement/s
#define FREQUENCY     5000             // ms between measurements

// Measurement array and buffer size
#ifndef INA_COUNT                      // Number if INA devices
#define INA_COUNT        3
#endif
#define RECORD_SIZE    (1+2*INA_COUNT)
#ifndef BUFFER_SIZE                    // Number of records in buffer
#define BUFFER_SIZE    128/sizeof(uint32_t)/RECORD_SIZE // 128 bytes
#endif

#endif
