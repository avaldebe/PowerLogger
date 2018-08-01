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

char *rtc_fmt(char *datestr, const char fmt){
  switch (fmt) {
#ifdef __STM32F1__
  case 'D': // long date
    sprintf(datestr, "%04d-%02u-%02u", 2000+now.year, now.month, now.day);
    break;
  case 'd': // short date
    sprintf(datestr, "%02u%02u%02u", now.year, now.month, now.day);
    break;
  case 'T': // long time
    sprintf(datestr, "%02u:%02u:%02u", now.hour, now.minute, now.second);
    break;
  case 't': // short time
    sprintf(datestr, "%02u%02u", now.hour, now.minute);
    break;
  case 'C': // file.csv
    sprintf(datestr, "%02u%02u%02u.csv", now.year, now.month, now.day);
    break;
#else
  case 'D': // long date
    sprintf(datestr, "%04d-%02u-%02u", now.year(), now.month(), now.day());
    break;
  case 'd': // short date
    sprintf(datestr, "%02d%02u%02u", now.year()-2000, now.month(), now.day());
    break;
  case 'T': // long time
    sprintf(datestr, "%02u:%02u:%02u", now.hour(), now.minute(), now.second());
    break;
  case 't': // short time
    sprintf(datestr, "%02u%02u", now.hour(), now.minute());
    break;
  case 'C': // YYMMDD.csv
    sprintf(datestr, "%02d%02u%02u.csv", now.year()-2000, now.month(), now.day());
    break;
#endif
  }
  return datestr;
}

#endif
