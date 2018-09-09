#ifdef HAST_RTC
#include "RTCutil.h"

// STM32F1 internal RTC
#define INTERNAL_32kHz  32768
#define INTERNAL_62kHz  62500

// supported external RTCs
#define DS1307  1307
#define DS3231  3231
#define PCF8583 8583
#define PCF8563 8563

#if   HAST_RTC == INTERNAL_32kHz || HAST_RTC == INTERNAL_62kHz
  #ifndef __STM32F1__
  #error "Internal RTC options only for STM32F1"
  #endif
  #include <RTClock.h>                    // builtin RTC
  #if   HAST_RTC == INTERNAL_32kHz        // LSE: low-speed external oscillator
    static RTClock rtc(RTCSEL_LSE);            // 32768 Hz cristal
  #elif HAST_RTC == INTERNAL_62kHz        // HSE: high-speed external oscillator
    static RTClock rtc(RTCSEL_HSE);            // 8 MHz/128 (62500 Hz)
  #endif
  static tm_t now;
  static uint32_t unixtime;
#elif HAST_RTC == DS1307 || HAST_RTC == DS3231 || HAST_RTC == PCF8583 || HAST_RTC == PCF8563
  #include <RTClib.h>                     // External RTC
  #if   HAST_RTC == DS1307
    static RTC_DS1307 rtc;
  #elif HAST_RTC == DS3231
    static RTC_DS3231 rtc;
  #elif HAST_RTC == PCF8583
    static RTC_PCF8583 rtc;
  #elif HAST_RTC == PCF8563
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
#if   HAST_RTC == INTERNAL_32kHz  || HAST_RTC == INTERNAL_62kHz
  rtc.getTime(now);
  unixtime = rtc.getTime();
#else
  now = rtc.now();
  unixtime = now.unixtime();
#endif
  return unixtime;
}

uint32_t rtc_now(uint32_t time){
#if   HAST_RTC == INTERNAL_32kHz  || HAST_RTC == INTERNAL_62kHz
  rtc.setTime(time);
#else
  rtc.adjust(DateTime(time));
#endif
  return rtc_now(); // update now/unixtime
}

char *rtc_fmt(const char fmt){
  static char str[16];               // long enough for 1 line
  switch (fmt) {
#if   HAST_RTC == INTERNAL_32kHz  || HAST_RTC == INTERNAL_62kHz
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
