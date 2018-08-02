#ifndef U8X8util_h
#define U8X8util_h

#ifdef HAST_U8X8
  #include <U8x8lib.h>
  extern U8X8LOG u8x8log;

  void u8x8_begin();
  inline void u8x8_clean(){ u8x8log.print("\f"); } // \f = form feed: clear the screen
#else
  inline void u8x8_begin(){}
  inline void u8x8_clean(){}
#endif

#endif
