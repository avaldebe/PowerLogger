#ifndef TERMutil_h
#define TERMutil_h

#ifdef HAST_U8X8
  // suported display driver
  #define SSD1305   1305
  #define SSD1306   1306
  #define SSD1309   1309
  #define SH1106    1106
  #define SH1107    1107
  #define UC1701    1701
  #define PCD8544   8544
  // suported display size
  #define x64y48    0x4030
  #define x84y48    0x5430
  #define x128y32   0x8020
  #define x128y64   0x8040
  #define x128y128  0x8080

  #ifndef DISPLAY_SIZE
  #error "Missing DISPLAY_SIZE flag, try again with -DDISPLAY_SIZE=0x8040"
  #endif
  #include <stdint.h>
  // decode DISPLAY_SIZE,
  // display.size:          hex defined by DISPLAY_SIZE flag (eg 0x8040)
  // display.pixel.width:   display width in pixels          (eg 128 pixels)
  // display.pixel.height:  display height in pixels         (eg  32 pixels)
  // display.text.cols:     max text cols (width/8)          (eg  16 cols)
  // display.text.rows      max text rows (height/8)         (eg   8 rows)
  const union {
    uint16_t size;
    struct { uint16_t height:8,  width:8;   } pixel;
    struct { uint16_t :3,rows:5, :3,cols:5; } text;
  } display = { DISPLAY_SIZE };

  #include <U8x8lib.h>
  #if   HAST_U8X8 == SSD1305 && DISPLAY_SIZE == x128y64
    #define TERMINAL_TYPE U8X8_SSD1305_128X64_ADAFRUIT_HW_I2C
  #elif HAST_U8X8 == SSD1305 && DISPLAY_SIZE == x128y32
    #define TERMINAL_TYPE U8X8_SSD1305_128X32_NONAME_HW_I2C
  #elif HAST_U8X8 == SSD1306 && DISPLAY_SIZE == x128y64
    #define TERMINAL_TYPE U8X8_SSD1306_128X64_NONAME_HW_I2C
  //#define TERMINAL_TYPE U8X8_SSD1306_128X64_VCOMH0_HW_I2C
  //#define TERMINAL_TYPE U8X8_SSD1306_128X64_ALT0_HW_I2C
  #elif HAST_U8X8 == SSD1306 && DISPLAY_SIZE == x128y32
    #define TERMINAL_TYPE U8X8_SSD1306_128X32_UNIVISION_HW_I2C
  #elif HAST_U8X8 == SSD1306 && DISPLAY_SIZE == x64y48  // WEMOS/LOLIN D1 mini OLED shield
    #define U8G2_SSD1306_64X48_ER_F_HW_I2C
  #elif HAST_U8X8 == SSD1309 && DISPLAY_SIZE == x128y64
    #define TERMINAL_TYPE U8X8_SSD1309_128X64_NONAME0_HW_I2C
  //#define TERMINAL_TYPE U8X8_SSD1309_128X64_NONAME2_HW_I2C
  #elif HAST_U8X8 == SH1106 && DISPLAY_SIZE == x128y64
    #define TERMINAL_TYPE U8X8_SH1106_128X64_NONAME_HW_I2C
  //#define TERMINAL_TYPE U8X8_SH1106_128X64_VCOMH0_HW_I2C
  //#define TERMINAL_TYPE U8X8_SH1106_128X64_WINSTAR_HW_I2C
  #elif HAST_U8X8 == SH1107 && DISPLAY_SIZE == x128y128
    #define TERMINAL_TYPE U8X8_SH1107_128X128_HW_I2C
  #elif HAST_U8X8 == UC1701 && DISPLAY_SIZE == x128y64 && defined(DISPLAY_HW_SPI)
    #define TERMINAL_TYPE U8X8_UC1701_MINI12864_4W_HW_SPI     // HW SPI
  #elif HAST_U8X8 == UC1701 && DISPLAY_SIZE == x128y64 && defined(DISPLAY_H2_SPI)
    #define TERMINAL_TYPE U8X8_UC1701_MINI12864_2ND_4W_HW_SPI // HW SPI2
  #elif HAST_U8X8 == UC1701 && DISPLAY_SIZE == x128y64
    #define TERMINAL_TYPE U8X8_UC1701_MINI12864_4W_SW_SPI     // SW SPI
  #elif HAST_U8X8 == PCD8544 && DISPLAY_SIZE == x84y48 && defined(DISPLAY_HW_SPI)  // Nokia 5110 LCD
    #define TERMINAL_TYPE U8X8_PCD8544_84X48_4W_HW_SPI        // HW SPI
  #elif HAST_U8X8 == PCD8544 && DISPLAY_SIZE == x84y48 && defined(DISPLAY_H2_SPI)  // Nokia 5110 LCD
    #define TERMINAL_TYPE U8X8_PCD8544_84X48_2ND_4W_HW_SPI    // HW SPI2
  #elif HAST_U8X8 == PCD8544 && DISPLAY_SIZE == x84y48           // Nokia 5110 LCD
    #define TERMINAL_TYPE U8X8_PCD8544_84X48_4W_SW_SPI        // SW SPI
  #else
    #error "Unknown U8X8 display"
  #endif
  extern TERMINAL_TYPE TERMINAL;

  void TERMINAL_begin();
  inline void TERMINAL_clear(uint32_t ms=0){ delay(ms);TERMINAL.clear(); }
  inline void TERMINAL_home(){ TERMINAL.home(); }
  void TERMINAL_toggle();
#else
  #ifdef NO_TERMINAL
    inline void TERMINAL_begin(){}
  #else
    #ifndef TERMINAL
    #define TERMINAL Serial
    #endif
    inline void TERMINAL_begin(uint32_t baudrate=57600){
      TERMINAL.begin(baudrate);        // 57600 for ATmega328p 3.3V 8Mhz
      while(!TERMINAL){ delay(10); }   // wait for USB Serial
    }
  #endif
  inline void TERMINAL_clear(uint32_t ms=0){ delay(ms); }
  inline void TERMINAL_home(){}
  inline void TERMINAL_toggle(){}
#endif

#endif
