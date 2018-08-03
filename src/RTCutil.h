#ifndef RTCutil_h
#define RTCutil_h

#include <stdint.h>
#include <stdio.h>
#include <Print.h>

#ifdef HAST_RTC
  #ifndef BUILD_TIME
  #error "Missing BUILD_TIME flag, try again with -DBUILD_TIME=$UNIX_TIME"
  #endif
  void rtc_init(Print* out);
  uint32_t rtc_now();
  uint32_t rtc_now(uint32_t time);
  char *rtc_fmt(const char fmt);
#else
  void rtc_init(Print* out){};
  void rtc_now(){};
#endif

#endif
