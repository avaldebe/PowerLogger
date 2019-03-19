#ifndef TERMutil_h
#define TERMutil_h
#include <Arduino.h>

#include <Arduino.h>
#include "TERMu8x8.h"
#ifdef TERM_U8X8
  #include <U8x8lib.h>
  extern TERM_U8X8 TERMINAL;
#elif defined(NO_TERMINAL) || defined(TERMINAL)
#else
  #define TERMINAL Serial
#endif

void TERMINAL_begin(uint32_t baudrate=57600);
void TERMINAL_toggle();

inline void TERMINAL_clear(uint32_t ms=0){
  delay(ms);
#ifdef TERM_U8X8
  TERMINAL.clear();
#endif
}

inline void TERMINAL_home(){
#ifdef TERM_U8X8
  TERMINAL.home();
#endif
}

#endif
