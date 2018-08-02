#ifdef HAST_U8X8
#include "U8X8util.h"

#if   HAST_U8X8 == SSD1305_128X64
  #define U8LOG_WIDTH   16
  #define U8LOG_HEIGHT   8
  U8X8_SSD1305_128X64_ADAFRUIT_HW_I2C   u8x8(/* reset=*/ U8X8_PIN_NONE);
#elif HAST_U8X8 == SSD1305_128X32
  #define U8LOG_WIDTH   16
  #define U8LOG_HEIGHT   4
  U8X8_SSD1305_128X32_NONAME_HW_I2C     u8x8(/* reset=*/ U8X8_PIN_NONE);
#elif HAST_U8X8 == SSD1306_128X64
  #define U8LOG_WIDTH   16
  #define U8LOG_HEIGHT   8
  U8X8_SSD1306_128X64_NONAME_HW_I2C     u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SSD1306_128X64_VCOMH0_HW_I2C     u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SSD1306_128X64_ALT0_HW_I2C       u8x8(/* reset=*/ U8X8_PIN_NONE);
#elif HAST_U8X8 == SSD1306_128X32
  #define U8LOG_WIDTH   16
  #define U8LOG_HEIGHT   4
  U8X8_SSD1306_128X32_UNIVISION_HW_I2C  u8x8(/* reset=*/ U8X8_PIN_NONE);
#elif HAST_U8X8 == SSD1309_128X64
  #define U8LOG_WIDTH   16
  #define U8LOG_HEIGHT   8
  U8X8_SSD1306_128X64_NONAME0_HW_I2C    u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SSD1306_128X64_NONAME2_HW_I2C    u8x8(/* reset=*/ U8X8_PIN_NONE);
#elif HAST_U8X8 == SH1106_128X64
  #define U8LOG_WIDTH   16
  #define U8LOG_HEIGHT   8
  U8X8_SH1106_128X64_NONAME_HW_I2C      u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SH1106_128X64_VCOMH0_HW_I2C      u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SH1106_128X64_WINSTAR_HW_I2C     u8x8(/* reset=*/ U8X8_PIN_NONE);
#elif HAST_U8X8 == SH1107_128X128
  #define U8LOG_WIDTH   16
  #define U8LOG_HEIGHT  16
  U8X8_SH1107_128X128_HW_I2C            u8x8(/* reset=*/ U8X8_PIN_NONE);
#endif

static uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];
U8X8LOG u8x8log;

void u8x8_begin(){
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  
  u8x8log.begin(u8x8, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8x8log.setRedrawMode(0);		// 0: Update screen with newline, 1: Update screen for every char  
}
#endif
