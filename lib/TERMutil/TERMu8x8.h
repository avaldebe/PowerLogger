#ifndef TERMu8x8_h
#define TERMu8x8_h

// suported display drivers
#define SSD1305   1305
#define SSD1306   1306
#define SSD1309   1309
#define SH1106    1106
#define SH1107    1107
#define UC1701    1701
#define PCD8544   8544

#if   defined(TERM_U8X8) && defined(DISPLAY_SIZE)
  // defined directly on platfromio.ini
#elif defined(DISPLAY_64X48)
  #define DISPLAY_SIZE 0x4030
  #ifdef TERM_U8X8
  #elif DISPLAY_64X48 == SSD1306  // WEMOS/LOLIN D1 mini OLED shield
    #define TERM_U8X8 U8X8_SSD1306_64X48_ER_F_HW_I2C
  #else
    #error "Unsuported DISPLAY_64X48"
  #endif
#elif defined(DISPLAY_84X48)
  #define DISPLAY_SIZE 0x5430
  #ifdef TERM_U8X8
  #elif DISPLAY_84X48 == PCD8544 && defined(DISPLAY_SW_SPI) // Nokia 5110 LCD, SW SPI
    #define TERM_U8X8 U8X8_PCD8544_84X48_4W_SW_SPI
  #elif DISPLAY_84X48 == PCD8544 && defined(DISPLAY_HW_SPI) // Nokia 5110 LCD, HW SPI
    #define TERM_U8X8 U8X8_PCD8544_84X48_4W_HW_SPI
  #elif DISPLAY_84X48 == PCD8544 && defined(DISPLAY_H2_SPI) // Nokia 5110 LCD, HW SPI2
    #define TERM_U8X8 U8X8_PCD8544_84X48_2ND_4W_HW_SPI
  #else
    #error "Unsuported DISPLAY_84X48"
  #endif
#elif defined(DISPLAY_128X32)
  #define DISPLAY_SIZE 0x8020
  #ifdef TERM_U8X8
  #elif DISPLAY_128X32 == SSD1305
    #define TERM_U8X8 U8X8_SSD1305_128X32_NONAME_HW_I2C
  #elif DISPLAY_128X32 == SSD1306
    #define TERM_U8X8 U8X8_SSD1306_128X32_UNIVISION_HW_I2C
  #else
    #error "Unsuported DISPLAY_128X32"
  #endif
#elif defined(DISPLAY_128X64)
  #define DISPLAY_SIZE 0x8040
  #ifdef TERM_U8X8
  #elif DISPLAY_128X64 == SSD1305
    #define TERM_U8X8 U8X8_SSD1305_128X64_ADAFRUIT_HW_I2C
  #elif DISPLAY_128X64 == SSD1306
    #define TERM_U8X8 U8X8_SSD1306_128X64_NONAME_HW_I2C
  //#define TERM_U8X8 U8X8_SSD1306_128X64_VCOMH0_HW_I2C
  //#define TERM_U8X8 U8X8_SSD1306_128X64_ALT0_HW_I2C
  #elif DISPLAY_128X64 == SSD1309
    #define TERM_U8X8 U8X8_SSD1309_128X64_NONAME0_HW_I2C
  //#define TERM_U8X8 U8X8_SSD1309_128X64_NONAME2_HW_I2C
  #elif DISPLAY_128X64 == SH1106
    #define TERM_U8X8 U8X8_SH1106_128X64_NONAME_HW_I2C
  //#define TERM_U8X8 U8X8_SH1106_128X64_VCOMH0_HW_I2C
  //#define TERM_U8X8 U8X8_SH1106_128X64_WINSTAR_HW_I2C
  #elif DISPLAY_128X64 == UC1701 && defined(DISPLAY_SW_SPI) // SW SPI
    #define TERM_U8X8 U8X8_UC1701_MINI12864_4W_SW_SPI
  #elif DISPLAY_128X64 == UC1701 && defined(DISPLAY_HW_SPI) // HW SPI
    #define TERM_U8X8 U8X8_UC1701_MINI12864_4W_HW_SPI
  #elif DISPLAY_128X64 == UC1701 && defined(DISPLAY_H2_SPI) // HW SPI2
    #define TERM_U8X8 U8X8_UC1701_MINI12864_2ND_4W_HW_SPI
  #else
    #error "Unsuported DISPLAY_128X64"
  #endif
#elif defined(DISPLAY_128X128)
  #define DISPLAY_SIZE 0x8080
  #ifdef TERM_U8X8
  #elif DISPLAY_128X128 == SH1107
    #define TERM_U8X8 U8X8_SH1107_128X128_HW_I2C
  #else
    #error "Unsuported DISPLAY_128X128"
  #endif
#endif

#ifndef DISPLAY_SIZE
  #ifdef TERM_U8X8
    #error "Missing DISPLAY_SIZE flag, e.g. -D DISPLAY_SIZE=0x8040"
  #else                           // e.g. Serial.print
    #define DISPLAY_SIZE 0xA0A0   // 20x20 characters
  #endif
#endif

#ifdef DISPLAY_SIZE
  // decode DISPLAY_SIZE
  // display.size:          hex defined by DISPLAY_SIZE flag (eg 0x8040)
  // display.pixel.width:   display width in pixels          (eg 128 pixels)
  // display.pixel.height:  display height in pixels         (eg  32 pixels)
  // display.text.cols:     max text cols (width/8)          (eg  16 cols)
  // display.text.rows      max text rows (height/8)         (eg   8 rows)
  #include <stdint.h>
  const union {
    uint16_t size;
    struct { uint16_t height:8,  width:8;   } pixel;
    struct { uint16_t :3,rows:5, :3,cols:5; } text;
  } display = { DISPLAY_SIZE };
#endif

#endif
