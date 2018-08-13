#include <Arduino.h>
#include <unity.h>

#include "../src/config.h"                // project configuration
#include <RTCutil.h>                      // internal/external RTC
#include <U8X8util.h>
#ifdef HAST_U8X8
  #define TERNIMAL(msg) u8x8log.println(msg)
#else
  #define TERNIMAL(msg)
#endif

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
  TEST_ASSERT_MESSAGE(SD.begin(chipSelect, SPI_SPEED), "SD.begin");

  TEST = SD.open(FILENAME, FILE_WRITE);
  TEST_ASSERT_MESSAGE(TEST, "SD.open");
  
  TEST.println(F("Testing 1,2,3"));
  TEST.close();
  TEST_ASSERT_MESSAGE(SD.exists(FILENAME), "SD.exists");
  TEST_ASSERT_MESSAGE(SD.remove(FILENAME), "SD.remove");
}

void test_RTC(void) {
#ifdef HAST_RTC
  if(rtc_now()<BUILD_TIME){ rtc_now(BUILD_TIME); } // set RTC if needed
  TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(BUILD_TIME, rtc_now(), "Stale RTC");
#else
  TEST_IGNORE_MESSAGE("No RTC, skip test")
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
  u8x8_begin();                             // start TERNIMAL
  button.setClickTicks(SHORTPRESS);         // single press duration [ms]
  button.setPressTicks(LONGPRESS);          // long press duration [ms]
  button.attachClick(setSinglePress);       // single press
  button.attachDoubleClick(setDoublePress); // double press
  button.attachPress(setLongPress);         // long press

  TERNIMAL(F("SinglePress"));
  button_wait();
  TEST_ASSERT_MESSAGE(press==SinglePress, "No SinglePress detected");

  TERNIMAL(F("DoublePress"));
  button_wait();
  TEST_ASSERT_MESSAGE(press==DoublePress, "No DoublePress detected");

  TERNIMAL(F("LongPress"));
  button_wait();
  TEST_ASSERT_MESSAGE(press==LongPress, "No LongPress detected");

  u8x8_clean();
}


void setup() {
  while(millis()<2000) { delay(10); }       // ensure Serial is available
  UNITY_BEGIN();

  RUN_TEST(test_INA);
  RUN_TEST(test_SD);
  RUN_TEST(test_RTC);
  RUN_TEST(test_UI);

  UNITY_END();
}

// do nothing
void loop() {}
