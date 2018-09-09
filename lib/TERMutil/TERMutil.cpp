#include "TERMutil.h"
#ifdef TERM_U8X8
  #ifdef DISPLAY_RST
    static const uint8_t reset = DISPLAY_RST;
  #else
    static const uint8_t reset = U8X8_PIN_NONE; // triger the right U8X8 constructor
  #endif
  #if   defined(DISPLAY_SW_SPI)               // SW SPI
    TERM_U8X8 TERMINAL(DISPLAY_SW_SPI,reset);
  #elif defined(DISPLAY_HW_SPI)               // HW SPI1
    TERM_U8X8 TERMINAL(DISPLAY_HW_SPI,reset);
  #elif defined(DISPLAY_H2_SPI)               // HW SPI2
    TERM_U8X8 TERMINAL(DISPLAY_H2_SPI,reset);
  #else
    TERM_U8X8 TERMINAL(reset);
  #endif
#endif

void TERMINAL_begin(uint32_t baudrate){
#ifdef TERM_U8X8
  TERMINAL.begin();
  TERMINAL.setFont(u8x8_font_chroma48medium8_r);
  TERMINAL.clear();
#elif definded(TERMINAL)
  TERMINAL.begin(baudrate);             // 57600 for ATmega328p 3.3V 8Mhz
  while(!TERMINAL){ delay(10); }        // wait for USB Serial
#endif
}

void TERMINAL_toggle() {
#ifdef TERM_U8X8
  static bool is_on = true;               // backlight on
  is_on ^= true;                          // same as !is_on
  // enable (1) or disable (0) power save mode for the display
  TERMINAL.setPowerSave(is_on?0:1);
#endif
}
