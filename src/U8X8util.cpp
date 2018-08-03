#ifdef HAST_U8X8
#include "U8X8util.h"

#if   HAST_U8X8 == 1305 && DISPLAY_SIZE == 12864
  U8X8_SSD1305_128X64_ADAFRUIT_HW_I2C   u8x8(/* reset=*/ U8X8_PIN_NONE);
#elif HAST_U8X8 == 1305 && DISPLAY_SIZE == 12832
  U8X8_SSD1305_128X32_NONAME_HW_I2C     u8x8(/* reset=*/ U8X8_PIN_NONE);
#elif HAST_U8X8 == 1306 && DISPLAY_SIZE == 12864
  U8X8_SSD1306_128X64_NONAME_HW_I2C     u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SSD1306_128X64_VCOMH0_HW_I2C     u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SSD1306_128X64_ALT0_HW_I2C       u8x8(/* reset=*/ U8X8_PIN_NONE);
#elif HAST_U8X8 == 1306 && DISPLAY_SIZE == 12832
  U8X8_SSD1306_128X32_UNIVISION_HW_I2C  u8x8(/* reset=*/ U8X8_PIN_NONE);
#elif HAST_U8X8 == 1309 && DISPLAY_SIZE == 12864
  U8X8_SSD1306_128X64_NONAME0_HW_I2C    u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SSD1306_128X64_NONAME2_HW_I2C    u8x8(/* reset=*/ U8X8_PIN_NONE);
#elif HAST_U8X8 == 1106 && DISPLAY_SIZE == 12864
  U8X8_SH1106_128X64_NONAME_HW_I2C      u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SH1106_128X64_VCOMH0_HW_I2C      u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SH1106_128X64_WINSTAR_HW_I2C     u8x8(/* reset=*/ U8X8_PIN_NONE);
#elif HAST_U8X8 == 1107 && DISPLAY_SIZE == 128128
  U8X8_SH1107_128X128_HW_I2C            u8x8(/* reset=*/ U8X8_PIN_NONE);
#endif

static const uint16_t width = (DISPLAY_SIZE/100)/8, height = (DISPLAY_SIZE%100)/8;
static uint8_t u8log_buffer[width*height];
U8X8LOG u8x8log;

void u8x8_begin(){
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  
  u8x8log.begin(u8x8, width, height, u8log_buffer);
  u8x8log.setRedrawMode(0);		// 0: Update screen with newline, 1: Update screen for every char  
}
#endif
