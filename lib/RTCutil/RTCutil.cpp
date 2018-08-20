#ifdef HAST_RTC
#include "RTCutil.h"

#if   HAST_RTC == 32768  || HAST_RTC == 62500
  #ifndef __STM32F1__
  #error "Internal RTC options only for STM32F1"
  #endif
  #include <RTClock.h>                    // builtin RTC
  #if   HAST_RTC == 32768                 // esternal clock: 32768 Hz cristal
    static RTClock rtc(RTCSEL_LSE);
  #elif HAST_RTC == 62500                 // internal clock: 8 MHz/128 (62500 Hz)
    static RTClock rtc(RTCSEL_HSE);
  #endif
  static tm_t now;
  static uint32_t unixtime;
#elif HAST_RTC == 1307 || HAST_RTC == 3231 || HAST_RTC == 8583 || HAST_RTC == 8563
  #include <RTClib.h>                     // External RTC
  #if   HAST_RTC == 1307
    static RTC_DS1307 rtc;
  #elif HAST_RTC == 3231
    static RTC_DS3231 rtc;
  #elif HAST_RTC == 8583
    static RTC_PCF8583 rtc;
  #elif HAST_RTC == 8563
    static RTC_PCF8563 rtc;
  #endif
  static DateTime now;
  static uint32_t unixtime;
#else
  #error "Unknown RTC option"
#endif

#ifndef BUILD_TIME
  #error "Missing BUILD_TIME flag, try again with -DBUILD_TIME=$UNIX_TIME"
#endif
bool rtc_stale(){ return rtc_now()<BUILD_TIME; }
void rtc_init(){ if(rtc_stale()) { rtc_now(BUILD_TIME); } }

uint32_t rtc_now(){
#if   HAST_RTC == 32768  || HAST_RTC == 62500
  rtc.getTime(now);
  unixtime = rtc.getTime();
#else
  now = rtc.now();
  unixtime = now.unixtime();
#endif
  return unixtime;
}

uint32_t rtc_now(uint32_t time){
#if   HAST_RTC == 32768  || HAST_RTC == 62500
  rtc.setTime(time);
#else
  rtc.adjust(DateTime(time));
#endif
  return rtc_now(); // update now/unixtime
}

char *rtc_fmt(const char fmt){
  static char str[16];               // long enough for 1 line
  switch (fmt) {
#if   HAST_RTC == 32768  || HAST_RTC == 62500
  case 'D': // long date
    sprintf(str, "%04u-%02u-%02u", 2000+now.year, now.month, now.day);
    break;
  case 'd': // short date
    sprintf(str, "%02u%02u%02u", now.year, now.month, now.day);
    break;
  case 'T': // long time
    sprintf(str, "%02u:%02u:%02u", now.hour, now.minute, now.second);
    break;
  case 't': // short time
    sprintf(str, "%02u%02u", now.hour, now.minute);
    break;
  case 'C': // file.csv
    sprintf(str, "%02u%02u%02u.csv", now.year, now.month, now.day);
    break;
#else
  case 'D': // long date
    sprintf(str, "%04u-%02u-%02u", now.year(), now.month(), now.day());
    break;
  case 'd': // short date
    sprintf(str, "%02d%02u%02u", now.year()-2000, now.month(), now.day());
    break;
  case 'T': // long time
    sprintf(str, "%02u:%02u:%02u", now.hour(), now.minute(), now.second());
    break;
  case 't': // short time
    sprintf(str, "%02u%02u", now.hour(), now.minute());
    break;
  case 'C': // YYMMDD.csv
    sprintf(str, "%02d%02u%02u.csv", now.year()-2000, now.month(), now.day());
    break;
#endif
  }
  return str;
}
#endif
