#ifndef RTCutil_h
#define RTCutil_h

#include <stdint.h>
#include <stdio.h>
#include <Print.h>

#ifdef HAST_RTC
  #ifndef BUILD_TIME
  #error "Missing BUILD_TIME flag, try again with -DBUILD_TIME=$UNIX_TIME"
  #endif
  uint32_t rtc_init();  // update RTC if needed
  uint32_t rtc_now();
  uint32_t rtc_now(uint32_t time);
  char *rtc_fmt(const char fmt, char *str);
#else
  inline uint32_t rtc_init(){ return 0; };
  inline uint32_t rtc_now(){ return 0; };
#endif

#endif
