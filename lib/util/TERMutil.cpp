#ifdef HAST_U8X8
#include "TERMutil.h"

static const uint8_t reset = U8X8_PIN_NONE; // triger the right U8X8 constructor
static const uint8_t width = (DISPLAY_SIZE >> 8  ) >> 3; // same as (DISPLAY_SIZE/256)/8
static const uint8_t height= (DISPLAY_SIZE & 0xFF) >> 3; // same as (DISPLAY_SIZE%256)/8

#if   HAST_U8X8 == 1305 && DISPLAY_SIZE == 0x8040
  U8X8_SSD1305_128X64_ADAFRUIT_HW_I2C   SCREEN(reset);
#elif HAST_U8X8 == 1305 && DISPLAY_SIZE == 0x8020
  U8X8_SSD1305_128X32_NONAME_HW_I2C     SCREEN(reset);
#elif HAST_U8X8 == 1306 && DISPLAY_SIZE == 0x8040
  U8X8_SSD1306_128X64_NONAME_HW_I2C     SCREEN(reset);
//U8X8_SSD1306_128X64_VCOMH0_HW_I2C     SCREEN(reset);
//U8X8_SSD1306_128X64_ALT0_HW_I2C       SCREEN(reset);
#elif HAST_U8X8 == 1306 && DISPLAY_SIZE == 0x8020
  U8X8_SSD1306_128X32_UNIVISION_HW_I2C  SCREEN(reset);
#elif HAST_U8X8 == 1306 && DISPLAY_SIZE == 0x4030  // WEMOS/LOLIN D1 mini OLED shield
  U8G2_SSD1306_64X48_ER_F_HW_I2C        SCREEN(reset);
#elif HAST_U8X8 == 1309 && DISPLAY_SIZE == 0x8040
  U8X8_SSD1306_128X64_NONAME0_HW_I2C    SCREEN(reset);
//U8X8_SSD1306_128X64_NONAME2_HW_I2C    SCREEN(reset);
#elif HAST_U8X8 == 1106 && DISPLAY_SIZE == 0x8040
  U8X8_SH1106_128X64_NONAME_HW_I2C      SCREEN(reset);
//U8X8_SH1106_128X64_VCOMH0_HW_I2C      SCREEN(reset);
//U8X8_SH1106_128X64_WINSTAR_HW_I2C     SCREEN(reset);
#elif HAST_U8X8 == 1107 && DISPLAY_SIZE == 0x8080
  U8X8_SH1107_128X128_HW_I2C            SCREEN(reset);
#elif HAST_U8X8 == 1701 && DISPLAY_SIZE == 0x8040
  #if defined DISPLAY_CS                // 1st hardware SPI
    U8X8_UC1701_MINI12864_4W_HW_SPI     SCREEN(DISPLAY_CS, MOSI, reset);
  #elif defined SS1 && defined MOSI1    // 2nd hardware SPI
    U8X8_UC1701_MINI12864_2ND_4W_HW_SPI SCREEN(SS1, MOSI1, reset);
  #else                                 // default
    U8X8_UC1701_MINI12864_4W_HW_SPI     SCREEN(SS, MOSI, reset);
  #endif
#elif HAST_U8X8 == 8544 && DISPLAY_SIZE == 0x5430  //  Nokia 5110 LCD
  #if defined DISPLAY_CS                // 1st hardware SPI
    U8X8_PCD8544_84X48_4W_HW_SPI        SCREEN(DISPLAY_CS, MOSI , reset);
  #elif defined SS1 && defined MOSI1    // 2nd hardware SPI
    U8X8_PCD8544_84X48_2ND_4W_HW_SPI    SCREEN(SS1, MOSI1, reset);
  #else                                 // default
    U8X8_PCD8544_84X48_4W_HW_SPI        SCREEN(SS, MOSI, reset);
  #endif
#else
  #error "Unknown U8X8 display"
#endif

static uint8_t u8log_buffer[width*height];
U8X8LOG TERMINAL;

void TERMINAL_begin(uint8_t mode){
  SCREEN.begin();
  SCREEN.setFont(u8x8_font_chroma48medium8_r);

  TERMINAL.begin(SCREEN, width, height, u8log_buffer);
  // Update screen (mode): 0 with newline, 1 for every char
  TERMINAL.setRedrawMode(mode);

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
