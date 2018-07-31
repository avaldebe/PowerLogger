#ifndef RTCutil_h
#define RTCutil_h

#ifndef HAST_RTC
  #error   "Missing DSHAST_RTC1307 flag, try agail with -DHAST_RTC=DS1307"
#endif

#ifndef BUILD_TIME
  #error   "Missing BUILD_TIME flag, try agail with -DBUILD_TIME=$UNIX_TIME"
#endif

#ifdef __STM32F1__                        // builtin RTC
  #include <RTClock.h>                      // internal RTC
  RTClock rtc(RTCSEL_LSE);                  // initialise clock source
  #define rtc_now rtc.getTime
  #define rtc_set rtc.setTime
#endif

#endif
