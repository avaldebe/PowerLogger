#ifdef HAST_U8X8
#include "TERMutil.h"

#ifdef DISPLAY_RST
static const uint8_t reset = DISPLAY_RST;
#else
static const uint8_t reset = U8X8_PIN_NONE; // triger the right U8X8 constructor
#endif
#if HAST_U8X8 != 1701 && HAST_U8X8 != 8544  // 1st hardware I2C
  TERMINAL_TYPE TERMINAL(reset);
#elif defined(DISPLAY_SW_SPI)               // SW SPI
  TERMINAL_TYPE TERMINAL(DISPLAY_SW_SPI,reset);
#elif defined(DISPLAY_HW_SPI)               // HW SPI1
  TERMINAL_TYPE TERMINAL(DISPLAY_HW_SPI,reset);
#elif defined(DISPLAY_H2_SPI)               // HW SPI2
  TERMINAL_TYPE TERMINAL(DISPLAY_H2_SPI,reset);
#else
  #warning "Missing DISPLAY_SW_SPI flag, e.g. -D DISPLAY_SW_SPI=clock,data,cs,dc"
  #warning "Missing DISPLAY_HW_SPI flag, e.g. -D DISPLAY_HW_SPI=cs,dc"
  #warning "Missing DISPLAY_H2_SPI flag, e.g. -D DISPLAY_H2_SPI=cs,dc"
  #error "Missing DISPLAY_??_SPI flags, set one according your pinout"
#endif

void TERMINAL_begin(){
  TERMINAL.begin();
  TERMINAL.setFont(u8x8_font_chroma48medium8_r);
  TERMINAL.clear();

#ifdef BACKLIGHT_PIN
  // display backlight attached/controlled by BACKLIGHT_PIN
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);  // backlight on
#endif
}

void TERMINAL_toggle() {
  static bool is_on = true;           // backlight on
  is_on ^= true; // same as !is_on
#ifdef BACKLIGHT_PIN
  // display backlight attached/controlled by BACKLIGHT_PIN
  digitalWrite(BACKLIGHT_PIN, is_on?HIGH:LOW);
#else
  // enable (1) or disable (0) power save mode for the display
  TERMINAL.setPowerSave(is_on?0:1);
#endif
}

#endif
