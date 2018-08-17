#ifndef TERMutil_h
#define TERMutil_h

#ifdef HAST_U8X8
  #ifndef DISPLAY_SIZE
  #error "Missing DISPLAY_SIZE flag, try again with -DDISPLAY_SIZE=0x8040"
  #endif

  #include <U8x8lib.h>
  extern U8X8LOG TERMINAL;

  void TERMINAL_begin(uint8_t mode=0);  // 0: Update screen with newline, 1: Update screen for every char
  inline void TERMINAL_clean(){ TERMINAL.print("\f"); } // \f = form feed: clear the screen
  void TERMINAL_toggle();
#else
  #ifdef NO_TERMINAL
    inline void TERMINAL_begin(uint8_t mode=0){}
  #else
    #ifndef TERMINAL
    #define TERMINAL Serial
    #endif
    inline void TERMINAL_begin(uint8_t mode=0, uint32_t baudrate=57600){
        TERMINAL.begin(baudrate);        // 57600 for ATmega328p 3.3V 8Mhz
        while(!TERMINAL){ delay(10); }   // wait for USB Serial
    }
  #endif
  inline void TERMINAL_clean(){}
  inline void TERMINAL_toggle(){}
#endif

#endif
