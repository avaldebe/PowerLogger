/*
PowerLogger
DIY multi-channel voltage/current data logger
https://github.com:avaldebe/PowerLogger/

Based on

- DisplayReadings example fom INA library
  https://github.com/SV-Zanshin/INA

- QuickStart example form the SdFat library
  https://github.com/greiman/SdFat

- Object example on the CircularBuffer library
  https://github.com/rlogiacco/CircularBuffer

- Terminal example from the U8g2 library
  https://github.com/olikraus/u8g2/

- BlinkMachine esmaple from the OneButton library
  https://github.com/mathertel/OneButton

- Unit Testing of a “Blink” Project
  http://docs.platformio.org/en/latest/tutorials/core/unit_testing_blink.html
*/

#include <Arduino.h>
#include <Wire.h>
#include <unity.h>

#include "../src/config.h"                // project configuration
#include <RTCutil.h>                      // internal/external RTC
#include <INAbufer.h>                     // secret sauce ;-)

#include <SdFat.h>
SdFat SD;                                 // File system object.
File TEST;                                // File object for filename
const char *FILENAME = "test.123";

#include <OneButton.h>
OneButton button(BUTTON_PIN, true);       // with INPUT_PULLUP

// TERMINAL for additional messages
#ifdef NO_TERMINAL
  // messages via TEST_ASSERT_MESSAGE
#elif defined(HAST_U8X8)
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

// PASS/FAIL messages to TERMINAL
#ifdef NO_TERMINAL
  #define TERM(msg)
  #define TERM_CLEAN(ms)
  #define TERM_FMEM(mem)
  #define TERM_FAIL(fail)
  #define TEST_TERM(ok, msg)  TEST_ASSERT_MESSAGE(ok, msg)
#else
  #define TERM(msg)           TERMINAL.println(msg)
  #define TERM_CLEAR(ms)      TERMINAL_clear(ms)
  #define TERM_FMEM(mem)      TERMINAL.print(F("MEM "));TERMINAL.println(mem, DEC)
  #define TERM_FAIL(fail)     TERMINAL.print((fail)?F("  Fail!\n"):F(""))
  #define TEST_TERM(ok, msg)  TERM_FAIL(!(ok)); TEST_ASSERT(ok)
#endif
#include <TERMutil.h>
#include <MemoryFree.h>

void test_MEM(void) {
  int mem = freeMemory();
  TERM_FMEM(mem/8);
  TEST_TERM(mem>0, "Not enough memory");
}

void test_INA(void) {
  TERM(F("INA"));

  TERM(F("  count"));
  uint8_t count = INA.begin(1,100000);
  TEST_TERM(INA_COUNT==count, "INA devices found");

  INA.setBusConversion(INA_CONVTIME);     // see config.h for value
  INA.setShuntConversion(INA_CONVTIME);   // see config.h for value
  INA.setAveraging(INA_SAMPLES);          // see config.h for value
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);  // bus&shunt

  TERM(F("  offset"));
  for (uint8_t i=0; i<INA_COUNT; i++) {
    uint32_t mv = INA.getBusMilliVolts(i);
    uint32_t ua = INA.getBusMicroAmps(i);
    TEST_TERM(mv==0, "INA.getBusMilliVolts !=0");
    TEST_TERM(ua==0, "INA.getBusMicroAmps !=0");
  }
}

void test_BUF(void) {
  TERM(F("BUF"));
  TERM(F("  fill"));
  Record* record = NULL;
  while (!buffer.isFull()) {
    record = new Record(millis());
    TEST_TERM(record, "Buffer too large");
    buffer.unshift(record);
  }
}

void test_SD(void) {
  TERM(F("SD"));

  TERM(F("  begin"));
  bool ok = SD.begin(SD_CS, SPI_SPEED);
  TEST_TERM(ok, "SD.begin");

  TERM(F("  open"));
  TEST = SD.open(FILENAME, FILE_WRITE);
  TEST_TERM(TEST, "SD.open");

  TERM(F("  write"));
  Record* record = NULL;
  record->header(&TEST);
  while (!buffer.isEmpty()) {
    record = buffer.shift();
    record->print(&TEST);
    delete record;
  }
  TEST.close();

  TERM(F("  exists"));
  ok = SD.exists(FILENAME);
  TEST_TERM(ok, "SD.exists");

  TERM(F("  remove"));
  ok = SD.remove(FILENAME);
  TEST_TERM(ok, "SD.remove");
}

void test_RTC(void) {
  TERM(F("RTC"));
#ifndef HAST_RTC
  TEST_IGNORE_MESSAGE("No RTC, skip test");
#else
#if HAST_RTC != 32768 && HAST_RTC != 62500
  TERM(F("  exists"));
  Wire.beginTransmission(0x68);
  uint8_t error = Wire.endTransmission();
  TEST_TERM(error==0, "RTC not found");
#endif
  TERM(F("  running"));
  rtc_init();  // update RTC if needed
  bool ok = !rtc_stale();
  TEST_TERM(ok, "Stale RTC");
#endif
}

enum Press { NoPress, SinglePress, DoublePress, LongPress } press = NoPress;
void setSinglePress(void) { press = SinglePress; }
void setDoublePress(void) { press = DoublePress; }
void setLongPress(void)   { press = LongPress; }
void button_wait(uint16_t wait_ms=2000) { // 1 sec max, by default
  press = NoPress;
  button.reset();
  uint32_t start = millis();
  while ((press==NoPress) && (millis()-start<wait_ms)) {
    button.tick();
    delay(10);
  }
}

void test_UI(void) {
  TERM(F("UI"));
#ifndef HAST_U8X8
  TEST_IGNORE_MESSAGE("No display, skip test");
#else
#ifdef SHORTPRESS
  button.setClickTicks(SHORTPRESS);         // single press duration [ms]
#endif
#ifdef LONGPRESS
  button.setPressTicks(LONGPRESS);          // long press duration [ms]
#endif
  button.attachClick(setSinglePress);       // single press
  button.attachDoubleClick(setDoublePress); // double press
  button.attachPress(setLongPress);         // long press

  TERM(F("  SinglePress"));
  button_wait();
  TEST_TERM(press==SinglePress, "No SinglePress detected");

  TERM(F("  DoublePress"));
  button_wait();
  TEST_TERM(press==DoublePress, "No DoublePress detected");

  TERM(F("  LongPress"));
  button_wait();
  TEST_TERM(press==LongPress, "No LongPress detected");
#endif
}


void setup() {
  TERMINAL_begin();                // start TERMINAL
  while (!Serial) { delay(10); }   // wait for USB Serial
  UNITY_BEGIN();
  RUN_TEST(test_MEM);

  TERM_CLEAR(1000);
  RUN_TEST(test_RTC);
  RUN_TEST(test_MEM);

  TERM_CLEAR(1000);
  RUN_TEST(test_INA);
  RUN_TEST(test_MEM);

  TERM_CLEAR(1000);
  RUN_TEST(test_UI);
  RUN_TEST(test_MEM);
}

// do nothing
void loop() {
  const uint8_t max_count = 3;
  static uint8_t count = 0;
  if (count++ < max_count) {
    TERM_CLEAR(1000);
    RUN_TEST(test_BUF);
    RUN_TEST(test_MEM);

    TERM_CLEAR(1000);
    RUN_TEST(test_SD);
    RUN_TEST(test_MEM);
  } else {
    UNITY_END(); // stop unit testing
  }
}
