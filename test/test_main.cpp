#include <Arduino.h>
#include <Wire.h>
#include <unity.h>

#include "../src/config.h"                // project configuration
#include <RTCutil.h>                      // internal/external RTC
#include <TERMutil.h>

#include <INA.h>
INA_Class INA;

#include <SdFat.h>
SdFat SD;                                 // File system object.
File TEST;                                // File object for filename
const char *FILENAME = "test.123";

#include <OneButton.h>
OneButton button(BUTTON_PIN, true);       // with INPUT_PULLUP

void test_INA(void) {
  TEST_ASSERT_EQUAL_MESSAGE(INA_COUNT, INA.begin(1,100000), "INA devices found");

  INA.setBusConversion(INA_CONVTIME);     // see config.h for value
  INA.setShuntConversion(INA_CONVTIME);   // see config.h for value
  INA.setAveraging(INA_SAMPLES);          // see config.h for value
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);  // bus&shunt

  for (uint8_t i=0; i<INA_COUNT; i++) {
    TEST_ASSERT_EQUAL_MESSAGE(0, INA.getBusMilliVolts(i), "INA.getBusMilliVolts !=0");
    TEST_ASSERT_EQUAL_MESSAGE(0, INA.getBusMicroAmps(i),  "INA.getBusMicroAmps !=0");
  }
}

void test_SD(void) {
  TEST_ASSERT_MESSAGE(SD.begin(SD_CS, SPI_SPEED), "SD.begin");

  TEST = SD.open(FILENAME, FILE_WRITE);
  TEST_ASSERT_MESSAGE(TEST, "SD.open");

  TEST.println(F("Testing 1,2,3"));
  TEST.close();
  TEST_ASSERT_MESSAGE(SD.exists(FILENAME), "SD.exists");
  TEST_ASSERT_MESSAGE(SD.remove(FILENAME), "SD.remove");
}

void test_RTC(void) {
#ifndef HAST_RTC
  TEST_IGNORE_MESSAGE("No RTC, skip test")
#elif   HAST_RTC == 32768  || HAST_RTC == 62500
  if(rtc_now()<BUILD_TIME){ rtc_now(BUILD_TIME); } // update RTC if needed
  TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(BUILD_TIME, rtc_now(), "Stale RTC");
#else
  Wire.begin();
  Wire.beginTransmission(0x68);
  uint8_t error = Wire.endTransmission();
  TEST_ASSERT_MESSAGE(error==0, "RTC not found");
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
  TEST_IGNORE_MESSAGE("No display, skip test")
#else
  TERMINAL_begin();                         // start TERMINAL
  button.setClickTicks(SHORTPRESS);         // single press duration [ms]
  button.setPressTicks(LONGPRESS);          // long press duration [ms]
  button.attachClick(setSinglePress);       // single press
  button.attachDoubleClick(setDoublePress); // double press
  button.attachPress(setLongPress);         // long press

  TERMINAL.println(F("SinglePress"));
  button_wait();
  TERMINAL.println((press==SinglePress)?F("PASS"):F("FAIL!"));
  TEST_ASSERT_MESSAGE(press==SinglePress, "No SinglePress detected");

  TERMINAL.println(F("DoublePress"));
  button_wait();
  TERMINAL.println((press==DoublePress)?F("PASS"):F("FAIL!"));
  TEST_ASSERT_MESSAGE(press==DoublePress, "No DoublePress detected");

  TERMINAL.println(F("LongPress"));
  button_wait();
  TERMINAL.println((press==LongPress)?F("PASS"):F("FAIL!"));
  TEST_ASSERT_MESSAGE(press==LongPress, "No LongPress detected");

  TERMINAL_clean();
#endif
}


void setup() {
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
