#ifndef RTCutil_h
#define RTCutil_h
#include <stdint.h>
#include <stdio.h>

#ifndef HAST_RTC
#error "Missing HAST_RTC flag, try agail with -DHAST_RTC=DS1307"
#endif

#ifndef BUILD_TIME
#error "Missing BUILD_TIME flag, try agail with -DBUILD_TIME=$UNIX_TIME"
#endif

uint32_t rtc_now();
uint32_t rtc_now(uint32_t time);
char *rtc_fmt(char *datestr, const char fmt);
#endif
