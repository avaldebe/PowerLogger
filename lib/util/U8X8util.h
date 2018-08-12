#ifndef U8X8util_h
#define U8X8util_h

#ifdef HAST_U8X8
  #ifndef DISPLAY_SIZE
  #error "Missing DISPLAY_SIZE flag, try again with -DDISPLAY_SIZE=12864"
  #endif
  #include <U8x8lib.h>
  extern U8X8LOG u8x8log;

  void u8x8_begin(uint8_t mode=0);  // 0: Update screen with newline, 1: Update screen for every char
  inline void u8x8_clean(){ u8x8log.print("\f"); } // \f = form feed: clear the screen
#else
  inline void u8x8_begin(uint8_t mode=0){};
  inline void u8x8_clean(){};
#endif

#endif
