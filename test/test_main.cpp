#include <Arduino.h>
#include <Wire.h>
#include <unity.h>

#include "../src/config.h"                // project configuration
#include <RTCutil.h>                      // internal/external RTC

#include <INA.h>
INA_Class INA;

#include <SdFat.h>
SdFat SD;                                 // File system object.
File TEST;                                // File object for filename
const char *FILENAME = "test.123";

#include <OneButton.h>
OneButton button(BUTTON_PIN, true);       // with INPUT_PULLUP

// TERMINAL for additional messages
#ifdef HAST_U8X8
  // messages to DISPLAY
#elif defined(HAVE_HWSERIAL1) || defined(__STM32F1__) || defined(ESP32)
  #define TERMINAL Serial1
#elif defined(ESP8266)
  #define TERMINAL MySerial
  #include <SoftwareSerial.h>
  SoftwareSerial TERMINAL(5, 4); // RX:D1, TX:D2
#else
  #define NO_TERMINAL
#endif
#include <TERMutil.h>

// PASS/FAIL messages to TERMINAL
#ifdef NO_TERMINAL
  #warning "No TERMINAL messages"
  #define TERM(msg)
  #define TEST_TERM(ok, msg) TEST_ASSERT_MESSAGE(ok, msg)
#else
  #define TERM(msg)          TERMINAL.println(msg)
  #define TERM_FAIL(fail)    if(fail){ TERM(F("  Fail!")); }
  #define TEST_TERM(ok, msg) TERM_FAIL(!(ok)); TEST_ASSERT_MESSAGE(ok, msg)
#endif

void test_INA(void) {
  TERM(F("INA count"));
  uint8_t count = INA.begin(1,100000);
  TEST_TERM(INA_COUNT==count, "INA devices found");

  INA.setBusConversion(INA_CONVTIME);     // see config.h for value
  INA.setShuntConversion(INA_CONVTIME);   // see config.h for value
  INA.setAveraging(INA_SAMPLES);          // see config.h for value
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);  // bus&shunt

  TERM(F("INA offset"));
  for (uint8_t i=0; i<INA_COUNT; i++) {
    uint32_t mv = INA.getBusMilliVolts(i);
    uint32_t ua = INA.getBusMicroAmps(i);
    TEST_TERM(mv==0, "INA.getBusMilliVolts !=0");
    TEST_TERM(ua==0, "INA.getBusMicroAmps !=0");
  }
}

void test_SD(void) {
  TERM(F("SD begin"));
  bool ok = SD.begin(SD_CS, SPI_SPEED);
  TEST_TERM(ok, "SD.begin");

  TERM(F("SD open"));
  TEST = SD.open(FILENAME, FILE_WRITE);
  TEST_TERM(TEST, "SD.open");

  TEST.println(F("Testing 1,2,3"));
  TEST.close();
  TERM(F("SD exists"));
  ok = SD.exists(FILENAME);
  TEST_TERM(ok, "SD.exists");
  ok = SD.remove(FILENAME);
  TEST_TERM(ok, "SD.remove");
}

void test_RTC(void) {
#ifndef HAST_RTC
  TEST_IGNORE_MESSAGE("No RTC, skip test");
#else
#if HAST_RTC != 32768 && HAST_RTC != 62500
  TERM(F("RTC exists"));
  Wire.beginTransmission(0x68);
  uint8_t error = Wire.endTransmission();
  TEST_TERM(error==0, "RTC not found");
#endif
  TERM(F("RTC running"));
  uint32_t now = rtc_now();
  if (now<BUILD_TIME) { // update RTC if needed
    rtc_now(BUILD_TIME);
    now=rtc_now();
  }
  TEST_TERM(now>=BUILD_TIME, "Stale RTC");
#endif
}

enum Press { NoPress, SinglePress, DoublePress, LongPress } press = NoPress;
void setSinglePress(void) { press = SinglePress; }
void setDoublePress(void) { press = DoublePress; }
void setLongPress(void)   { press = LongPress; }
void button_wait(uint16_t wait_ms=1000) { // 1 sec max, by default
  press = NoPress;
  button.reset();
  uint32_t start = millis();
  while ((press==NoPress) && (millis()-start<wait_ms)) {
    button.tick();
    delay(10);
  }
}

void test_UI(void) {
#ifndef HAST_U8X8
  TEST_IGNORE_MESSAGE("No display, skip test");
#else
  button.setClickTicks(SHORTPRESS);         // single press duration [ms]
  button.setPressTicks(LONGPRESS);          // long press duration [ms]
  button.attachClick(setSinglePress);       // single press
  button.attachDoubleClick(setDoublePress); // double press
  button.attachPress(setLongPress);         // long press

  TERM(F("UI SinglePress"));
  button_wait();
  TEST_TERM(press==SinglePress, "No SinglePress detected");

  TERM(F("UI DoublePress"));
  button_wait();
  TEST_TERM(press==DoublePress, "No DoublePress detected");

  TERM(F("UI LongPress"));
  button_wait();
  TEST_TERM(press==LongPress, "No LongPress detected");
#endif
}


void setup() {
  TERMINAL_begin();                // start TERMINAL
  while (!Serial) { delay(10); }   // wait for USB Serial
  UNITY_BEGIN();

  RUN_TEST(test_INA);
  RUN_TEST(test_SD);
  RUN_TEST(test_RTC);
  RUN_TEST(test_UI);

  UNITY_END();
}

// do nothing
void loop() {}
