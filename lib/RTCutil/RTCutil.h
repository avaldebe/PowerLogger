#ifndef RTCutil_h
#define RTCutil_h

#include <stdint.h>
#include <stdio.h>
#include <Print.h>

#ifdef HAST_RTC
  bool rtc_stale();
  uint32_t rtc_init();
  uint32_t rtc_now();
  uint32_t rtc_now(uint32_t time);
  char *rtc_fmt(const char fmt);  // returns internal buffer
#else
  inline uint32_t rtc_init(){ return 0; };
  inline uint32_t rtc_now(){ return 0; };
#endif

#endif
