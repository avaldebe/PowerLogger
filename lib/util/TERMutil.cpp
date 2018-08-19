#ifdef HAST_U8X8
#include "TERMutil.h"

static const uint8_t reset = U8X8_PIN_NONE; // triger the right U8X8 constructor
static const uint8_t width = (DISPLAY_SIZE >> 8  ) >> 3; // same as (DISPLAY_SIZE/256)/8
static const uint8_t height= (DISPLAY_SIZE & 0xFF) >> 3; // same as (DISPLAY_SIZE%256)/8

#if HAST_U8X8 == 1701 ||HAST_U8X8 == 8544
  #if defined DISPLAY_CS                // 1st hardware SPI
    SCREEN_TYPE SCREEN(DISPLAY_CS, MOSI, reset);
  #elif defined SS1 && defined MOSI1    // 2nd hardware SPI
    SCREEN_TYPE SCREEN(SS1, MOSI1, reset);
  #else                                 // default
    SCREEN_TYPE SCREEN(SS, MOSI, reset);
  #endif
#else
  SCREEN_TYPE   SCREEN(reset);
#endif

static uint8_t u8log_buffer[width*height];
U8X8LOG TERMINAL;

void TERMINAL_begin(uint8_t mode){
  SCREEN.begin();
  SCREEN.setFont(u8x8_font_chroma48medium8_r);

  TERMINAL.begin(SCREEN, width, height, u8log_buffer);
  // Update screen (mode): 0 with newline, 1 for every char
  TERMINAL.setRedrawMode(mode);
  TERMINAL_clean();

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
  SCREEN.setPowerSave(is_on?0:1);
#endif
}

#endif
