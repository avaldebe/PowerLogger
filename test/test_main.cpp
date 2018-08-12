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
}

void test_RTC(void) {
#ifdef HAST_RTC
  if(rtc_now()<BUILD_TIME){ rtc_now(BUILD_TIME); } // set RTC if needed
  TEST_ASSERT_LESS_THAN_MESSAGE(BUILD_TIME, rtc_now(), "Stale RTC");
#else
  TEST_IGNORE_MESSAGE("No RTC, skip test")
#endif
}

bool singlePress = false; void setSinglePress(void) { singlePress = true; }
bool doublePress = false; void setDoublePress(void) { doublePress = true; }
bool longPress   = false; void setLongPress(void)   { longPress   = true; }

void test_UI(void) {
  u8x8_begin();                           // start TERNIMAL
  uint32_t last;

  TERNIMAL(F("singlePress"));
  button.reset();
  last = millis();
  while (millis()-last<1000) {  // 1 sec max
    if (singlePress) { break; } // exit loop
    button.tick();
    delay(10);
  }
  TEST_ASSERT_MESSAGE(singlePress, "No singlePress detected");

  TERNIMAL(F("doublePress"));
  button.reset();
  last = millis();
  while (millis()-last<1000) {  // 1 sec max
    if (doublePress) { break; } // exit loop
    button.tick();
    delay(10);
  }
  TEST_ASSERT_MESSAGE(doublePress, "No doublePress detected");

  TERNIMAL(F("longPress"));
  button.reset();
  last = millis();
  while (millis()-last<1000) {  // 1 sec max
    if (longPress) { break; }   // exit loop
    button.tick();
    delay(10);
  }
  TEST_ASSERT_MESSAGE(longPress, "No longPress detected");

  u8x8_clean();
}


void setup() {
  button.setClickTicks(SHORTPRESS);         // single press duration [ms]
  button.setPressTicks(LONGPRESS);          // long press duration [ms]
  button.attachClick(setSinglePress);       // single press
  button.attachDoubleClick(setDoublePress); // double press
  button.attachPress(setLongPress);         // long press

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
