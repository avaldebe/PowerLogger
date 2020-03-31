#ifndef config_h
#define config_h
#include <stdint.h>

/* SD CARD: Reduced SPI speed for breadboards */
#ifdef BREADBOARD
#define SPI_SPEED SD_SCK_MHZ(4) // highest speed supported by the board under 4 MHz.
#else
#define SPI_SPEED SD_SCK_MHZ(50) // best performance
#endif
#ifndef SD_CS
#define SD_CS SS
#endif
// check for pin collision with TFT chip select pin
#if defined DISPLAY_CS && DISPLAY_CS == SD_CS
#error "DISPLAY_CS and SD_CS need different SS pins"
#endif

// Measurament frequency
#define INA_CONVTIME 8600 // maximum conversion time [us]: \
                          // 8.510ms on INA219, 8.244ms elsewhere
#define SAMPLES(frec, conv) ((uint16_t)frec * 1000 / (conv * 2))
#define INA_SAMPLES SAMPLES(FREQUENCY, INA_CONVTIME) // fit FREQUENCY
#ifndef FREQUENCY                                    // ms between measurements
#define FREQUENCY 1000                               // default
#endif
/*
#ifndef SHORTPRESS                     // ms single press duration
#define SHORTPRESS     100             // default
#endif
#ifndef LONGPRESS                      // ms long press duration
#define LONGPRESS      500             // default
#endif
*/

// Measurement array and buffer size
#ifndef BUFFER_SIZE     // Max buffer size
#define BUFFER_SIZE 128 // bytes
#endif

#endif
