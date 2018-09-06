#ifndef TERMutil_h
#define TERMutil_h

#ifdef HAST_U8X8
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
  #if   HAST_U8X8 == 1305 && DISPLAY_SIZE == 0x8040
    #define TERMINAL_TYPE U8X8_SSD1305_128X64_ADAFRUIT_HW_I2C
  #elif HAST_U8X8 == 1305 && DISPLAY_SIZE == 0x8020
    #define TERMINAL_TYPE U8X8_SSD1305_128X32_NONAME_HW_I2C
  #elif HAST_U8X8 == 1306 && DISPLAY_SIZE == 0x8040
    #define TERMINAL_TYPE U8X8_SSD1306_128X64_NONAME_HW_I2C
  //#define TERMINAL_TYPE U8X8_SSD1306_128X64_VCOMH0_HW_I2C
  //#define TERMINAL_TYPE U8X8_SSD1306_128X64_ALT0_HW_I2C
  #elif HAST_U8X8 == 1306 && DISPLAY_SIZE == 0x8020
    extern U8X8_SSD1306_128X32_UNIVISION_HW_I2C  TERMINAL;
  #elif HAST_U8X8 == 1306 && DISPLAY_SIZE == 0x4030  // WEMOS/LOLIN D1 mini OLED shield
    extern U8G2_SSD1306_64X48_ER_F_HW_I2C        TERMINAL;
  #elif HAST_U8X8 == 1309 && DISPLAY_SIZE == 0x8040
    #define TERMINAL_TYPE U8X8_SSD1306_128X64_NONAME0_HW_I2C
  //#define TERMINAL_TYPE U8X8_SSD1306_128X64_NONAME2_HW_I2C
  #elif HAST_U8X8 == 1106 && DISPLAY_SIZE == 0x8040
    #define TERMINAL_TYPE U8X8_SH1106_128X64_NONAME_HW_I2C
  //#define TERMINAL_TYPE U8X8_SH1106_128X64_VCOMH0_HW_I2C
  //#define TERMINAL_TYPE U8X8_SH1106_128X64_WINSTAR_HW_I2C
  #elif HAST_U8X8 == 1107 && DISPLAY_SIZE == 0x8080
    #define TERMINAL_TYPE U8X8_SH1107_128X128_HW_I2C
  #elif HAST_U8X8 == 1701 && DISPLAY_SIZE == 0x8040 && defined DISPLAY_CS
    #define TERMINAL_TYPE U8X8_UC1701_MINI12864_4W_HW_SPI     // 1st hardware SPI
  #elif HAST_U8X8 == 1701 && DISPLAY_SIZE == 0x8040 && defined SS1 && defined MOSI1
    #define TERMINAL_TYPE U8X8_UC1701_MINI12864_2ND_4W_HW_SPI // 2nd hardware SPI
  #elif HAST_U8X8 == 1701 && DISPLAY_SIZE == 0x8040
    #define TERMINAL_TYPE U8X8_UC1701_MINI12864_4W_SW_SPI     // hard coded SW SPI
  #elif HAST_U8X8 == 8544 && DISPLAY_SIZE == 0x5430 && defined DISPLAY_CS
    #define TERMINAL_TYPE U8X8_PCD8544_84X48_4W_HW_SPI        //  Nokia 5110 LCD, 1st hardware SPI
  #elif HAST_U8X8 == 8544 && DISPLAY_SIZE == 0x5430 && defined SS1 && defined MOSI1
    #define TERMINAL_TYPE U8X8_PCD8544_84X48_2ND_4W_HW_SPI    //  Nokia 5110 LCD, 2nd hardware SPI
  #elif HAST_U8X8 == 8544 && DISPLAY_SIZE == 0x5430
    #define TERMINAL_TYPE U8X8_PCD8544_84X48_4W_SW_SPI        // hard coded SW SPI
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
