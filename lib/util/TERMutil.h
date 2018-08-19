#ifndef TERMutil_h
#define TERMutil_h

#ifdef HAST_U8X8
  #ifndef DISPLAY_SIZE
  #error "Missing DISPLAY_SIZE flag, try again with -DDISPLAY_SIZE=0x8040"
  #endif

  #include <U8x8lib.h>
  #if   HAST_U8X8 == 1305 && DISPLAY_SIZE == 0x8040
    #define SCREEN_TYPE U8X8_SSD1305_128X64_ADAFRUIT_HW_I2C
  #elif HAST_U8X8 == 1305 && DISPLAY_SIZE == 0x8020
    #define SCREEN_TYPE U8X8_SSD1305_128X32_NONAME_HW_I2C
  #elif HAST_U8X8 == 1306 && DISPLAY_SIZE == 0x8040
    #define SCREEN_TYPE U8X8_SSD1306_128X64_NONAME_HW_I2C
  //#define SCREEN_TYPE U8X8_SSD1306_128X64_VCOMH0_HW_I2C
  //#define SCREEN_TYPE U8X8_SSD1306_128X64_ALT0_HW_I2C
  #elif HAST_U8X8 == 1306 && DISPLAY_SIZE == 0x8020
    extern U8X8_SSD1306_128X32_UNIVISION_HW_I2C  SCREEN;
  #elif HAST_U8X8 == 1306 && DISPLAY_SIZE == 0x4030  // WEMOS/LOLIN D1 mini OLED shield
    extern U8G2_SSD1306_64X48_ER_F_HW_I2C        SCREEN;
  #elif HAST_U8X8 == 1309 && DISPLAY_SIZE == 0x8040
    #define SCREEN_TYPE U8X8_SSD1306_128X64_NONAME0_HW_I2C
  //#define SCREEN_TYPE U8X8_SSD1306_128X64_NONAME2_HW_I2C
  #elif HAST_U8X8 == 1106 && DISPLAY_SIZE == 0x8040
    #define SCREEN_TYPE U8X8_SH1106_128X64_NONAME_HW_I2C
  //#define SCREEN_TYPE U8X8_SH1106_128X64_VCOMH0_HW_I2C
  //#define SCREEN_TYPE U8X8_SH1106_128X64_WINSTAR_HW_I2C
  #elif HAST_U8X8 == 1107 && DISPLAY_SIZE == 0x8080
    #define SCREEN_TYPE U8X8_SH1107_128X128_HW_I2C
  #elif HAST_U8X8 == 1701 && DISPLAY_SIZE == 0x8040
    #if defined DISPLAY_CS                // 1st hardware SPI
      #define SCREEN_TYPE U8X8_UC1701_MINI12864_4W_HW_SPI
    #elif defined SS1 && defined MOSI1    // 2nd hardware SPI
      #define SCREEN_TYPE U8X8_UC1701_MINI12864_2ND_4W_HW_SPI
    #else                                 // default
      #define SCREEN_TYPE U8X8_UC1701_MINI12864_4W_HW_SPI
    #endif
  #elif HAST_U8X8 == 8544 && DISPLAY_SIZE == 0x5430  //  Nokia 5110 LCD
    #if defined DISPLAY_CS                // 1st hardware SPI
      #define SCREEN_TYPE U8X8_PCD8544_84X48_4W_HW_SPI
    #elif defined SS1 && defined MOSI1    // 2nd hardware SPI
      #define SCREEN_TYPE U8X8_PCD8544_84X48_2ND_4W_HW_SPI
    #else                                 // default
      #define SCREEN_TYPE U8X8_PCD8544_84X48_4W_HW_SPI
    #endif
  #else
    #error "Unknown U8X8 display"
  #endif
  extern SCREEN_TYPE   SCREEN;
  extern U8X8LOG TERMINAL;

  void TERMINAL_begin(uint8_t mode=0);  // 0: Update screen with newline, 1: Update screen for every char
  inline void TERMINAL_clean(){ TERMINAL.print("\f"); } // \f = form feed: clear the screen
  void TERMINAL_toggle();
#else
  #ifdef NO_TERMINAL
    inline void TERMINAL_begin(uint8_t mode=0){}
  #else
    #ifndef TERMINAL
    #define TERMINAL Serial
    #endif
    inline void TERMINAL_begin(uint8_t mode=0, uint32_t baudrate=57600){
        TERMINAL.begin(baudrate);        // 57600 for ATmega328p 3.3V 8Mhz
        while(!TERMINAL){ delay(10); }   // wait for USB Serial
    }
  #endif
  inline void TERMINAL_top(){}
  inline void TERMINAL_clean(){}
  inline void TERMINAL_toggle(){}
#endif

#endif
