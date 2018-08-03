#ifdef HAST_U8X8
#include "U8X8util.h"

static const uint8_t reset = U8X8_PIN_NONE;
static const uint8_t width = (DISPLAY_SIZE/100)/8; 
static const uint8_t height= (DISPLAY_SIZE%100)/8;

#if   HAST_U8X8 == 1305 && DISPLAY_SIZE == 12864
  U8X8_SSD1305_128X64_ADAFRUIT_HW_I2C   u8x8(reset);
#elif HAST_U8X8 == 1305 && DISPLAY_SIZE == 12832
  U8X8_SSD1305_128X32_NONAME_HW_I2C     u8x8(reset);
#elif HAST_U8X8 == 1306 && DISPLAY_SIZE == 12864
  U8X8_SSD1306_128X64_NONAME_HW_I2C     u8x8(reset);
//U8X8_SSD1306_128X64_VCOMH0_HW_I2C     u8x8(reset);
//U8X8_SSD1306_128X64_ALT0_HW_I2C       u8x8(reset);
#elif HAST_U8X8 == 1306 && DISPLAY_SIZE == 12832
  U8X8_SSD1306_128X32_UNIVISION_HW_I2C  u8x8(reset);
#elif HAST_U8X8 == 1306 && DISPLAY_SIZE ==  6448  // WEMOS/LOLIN D1 mini OLED shield
  U8G2_SSD1306_64X48_ER_F_HW_I2C        u8x8(reset);
#elif HAST_U8X8 == 1309 && DISPLAY_SIZE == 12864
  U8X8_SSD1306_128X64_NONAME0_HW_I2C    u8x8(reset);
//U8X8_SSD1306_128X64_NONAME2_HW_I2C    u8x8(reset);
#elif HAST_U8X8 == 1106 && DISPLAY_SIZE == 12864
  U8X8_SH1106_128X64_NONAME_HW_I2C      u8x8(reset);
//U8X8_SH1106_128X64_VCOMH0_HW_I2C      u8x8(reset);
//U8X8_SH1106_128X64_WINSTAR_HW_I2C     u8x8(reset);
#elif HAST_U8X8 == 1107 && DISPLAY_SIZE == 128128
  U8X8_SH1107_128X128_HW_I2C            u8x8(reset);
#else
  #error "Unknown U8X8 display"
#endif

static uint8_t u8log_buffer[width*height];
U8X8LOG u8x8log;

void u8x8_begin(){
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  
  u8x8log.begin(u8x8, width, height, u8log_buffer);
  u8x8log.setRedrawMode(0);		// 0: Update screen with newline, 1: Update screen for every char  
}
#endif
