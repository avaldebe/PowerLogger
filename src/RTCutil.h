#ifndef RTCutil_h
#define RTCutil_h

#ifndef HAST_RTC
  #error "Missing DSHAST_RTC1307 flag, try agail with -DHAST_RTC=DS1307"
#endif

#ifndef BUILD_TIME
  #error "Missing BUILD_TIME flag, try agail with -DBUILD_TIME=$UNIX_TIME"
#endif

#ifdef __STM32F1__
  #include <RTClock.h>                    // builtin RTC
  RTClock rtc(RTCSEL_LSE);                // initialise clock source
  tm_t now;
#else
  #include <RTClib.h>
  #if   HAST_RTC == DS1307
  RTC_DS1307 rtc;
  #elif HAST_RTC == DS3231
  RTC_DS3231 rtc;
  #elif HAST_RTC == PCF8583
  RTC_PCF8583 rtc;
  #elif HAST_RTC == PCF8563
  RTC_PCF8563 rtc;
  #endif
  DateTime now;
#endif
uint32_t unixtime;

void rtc_now(){
#ifdef __STM32F1__
  rtc.getTime(now);
  unixtime = rtc.getTime();
#else
  now = rtc.now();
  unixtime = now.unixtime();
#endif
}

void rtc_set(uint32_t time){
#ifdef __STM32F1__
  rtc.setTime(time);
#else
  now = DateTime(time);
  rtc.adjust(now);
#endif
}

uint8_t rtc_get(const char fld){
  switch (fld) {
  case 'Y':
  case 'y':
  #ifdef __STM32F1__
    return now.year;
  #else
    return (uint8_t)(now.year()-2000);
  #endif
  case 'm':
  #ifdef __STM32F1__
    return now.month;
  #else
    return now.month();
  #endif
  case 'd':
  #ifdef __STM32F1__
    return now.day;
  #else
    return now.day();
  #endif
  case 'H':
  #ifdef __STM32F1__
    return now.hour;
  #else
    return now.hour();
  #endif
  case 'M':
  #ifdef __STM32F1__
    return now.minute;
  #else
    return now.minute();
  #endif
  case 'S':
  #ifdef __STM32F1__
    return now.second;
  #else
    return now.second();
  #endif
  default:
    return 0xFF;
  }
}



#endif
