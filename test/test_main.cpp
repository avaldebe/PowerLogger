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

#include <RTCutil.h>                      // internal/external RTC
#include <INAbufer.h>                     // secret sauce ;-)

#include <SdFat.h>
SdFat SD;                                 // File system object.
File TEST;                                // File object for filename
const char *FILENAME = "test.123";

#include <OneButton.h>
OneButton button(BUTTON_PIN, true);       // with INPUT_PULLUP

// TERMINAL for additional messages
#include <TERMu8x8.h>
#if   defined(TERM_U8X8) || defined(NO_TERMINAL)
  // TERM_U8X8:   messages to DISPLAY
  // NO_TERMINAL: messages via TEST_ASSERT_MESSAGE
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
  #include <TERMutil.h>
  #define TERM(msg)           TERMINAL.println(msg)
  #define TERM_CLEAR(ms)      TERMINAL_clear(ms)
  #define TERM_FMEM(mem)      TERMINAL.print(F("MEM "));TERMINAL.println(mem, DEC)
  #define TERM_FAIL(fail)     TERMINAL.print((fail)?F("  Fail!\n"):F(""))
  #define TEST_TERM(ok, msg)  TERM_FAIL(!(ok)); TEST_ASSERT(ok)
#endif
#include <MemoryFree.h>

void test_MEM(void) {
  int mem = freeMemory();
  TERM_FMEM(mem/8);
  TEST_TERM(mem>0, "Not enough memory");
}

void test_INA(void) {
  TERM(F("INA"));

  TERM(F("  count"));
  uint8_t count = Record::init();
  TEST_TERM(INA_COUNT==count, "INA devices found");

  TERM(F("  offset"));
  Record record(millis());
  for (uint8_t i=0; i<INA_COUNT; i++) {
    uint32_t mv = record.getMilliVolts(i);
    uint32_t ua = record.getMicroAmps(i);
    TEST_TERM(mv==0, "INA mV != 0");
    TEST_TERM(ua==0, "INA uA != 0");
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
  #ifdef INTERNAL_RTC
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

void test_Display(void) {
  TERM(F("Display"));
#ifndef TERM_U8X8
  TEST_IGNORE_MESSAGE("No display, skip test");
#else
  TERMINAL.print(F("size: 0x"));
  TERMINAL.println(display.size, HEX);
  TERMINAL.print(F("pixs: "));
  TERMINAL.print(display.pixel.width);
  TERMINAL.print(F("x"));
  TERMINAL.println(display.pixel.height);
  TERMINAL.print(F("text: "));
  TERMINAL.print(display.text.cols);
  TERMINAL.print(F("x"));TERMINAL.println(display.text.rows);

  TERM(F("  width"));
  bool ok = TERMINAL.getCols() == display.text.cols;
  TEST_TERM(ok, "Wrong display width");

  TERM(F("  height"));
  ok = TERMINAL.getRows() == display.text.rows;
  TEST_TERM(ok, "Wrong display height");

  TERMINAL.drawGlyph(display.text.cols-1, 0, 'C');
  TERMINAL.drawGlyph(0, display.text.rows-1, 'R');
#endif
}

void test_UI(void) {
  TERM(F("UI"));
#ifndef TERM_U8X8
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
  RUN_TEST(test_Display);
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
