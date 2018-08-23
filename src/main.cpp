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
#include "config.h"                       // project configuration
#include <INAbufer.h>                     // secret sauce ;-)

#include <SdFat.h>
SdFat SD;                                 // File system object.
File CSV;                                 // File object for filename

#include <RTCutil.h>
#ifdef HAST_RTC
  #define FILENAME rtc_fmt('C')           // 'YYMMDD.csv'
#else
  #define FILENAME "INA.csv"
#endif

#include <TERMutil.h>
void sd_dump();

#include <OneButton.h>
#ifndef BUTTON_PIN
  #error "Undefined BUTTON_PIN flag. Try with -DBUTTON_PIN=3"
#endif
OneButton button(BUTTON_PIN, true);       // with INPUT_PULLUP

bool recording = false;                   // write to SD (or not)
void recording_toggle();
void safe_shutdown();

void setup() {
  rtc_init();                             // set RTC if needed
#ifdef SHORTPRESS
  button.setClickTicks(SHORTPRESS);       // single press duration [ms]
#endif
#ifdef LONGPRESS
  button.setPressTicks(LONGPRESS);        // long press duration [ms]
#endif
  button.attachClick(recording_toggle);   // pause/resume buffering
  button.attachPress(safe_shutdown);      // dump buffen and power down
  button.attachDoubleClick(TERMINAL_toggle);  // switch backlight/display on/off

  TERMINAL_begin();                       // start TERMINAL

  // 1st splash page
  TERMINAL.println(F("PowerLogger"));
#ifdef GIT_REV
  TERMINAL.println(F("\nRev:"));
  TERMINAL.println(F(GIT_REV));
#endif
  TERMINAL.println(F("\nBuild:"));
  TERMINAL.println(F(__DATE__));
  TERMINAL.println(F(__TIME__));
  TERMINAL_clear(2000);                   // wait 2 sec before continuing

  // 2nd splash page
  TERMINAL.print(F("RTC @"));
  TERMINAL.println(rtc_now());

  Record::init(&TERMINAL);                // init/config INA devices
  if (!SD.begin(SD_CS, SPI_SPEED)) {
    SD.initErrorHalt(&TERMINAL);          // errorcode/message to TERMINAL
  }
  TERMINAL.print(F("SD: "));
  TERMINAL.println(FILENAME);
  if (!recording) {                       // wait until SD is resumed
    TERMINAL.println(F("SD: paused"));
  }
  TERMINAL_clear(2000);                   // wait 2 sec before continuing
}

void loop() {
  static uint32_t last = 0;
  do { // test at least once
    // check for button presses untill is time for a new Record
    button.tick();
    delay(10);
  } while (millis()-last<FREQUENCY);
  last = millis();

  // measurements from all INA devices
  Record* record = new Record(last);
  if (!record) {
    TERMINAL.println(F("Out of memeory"));
    return;
  }
  TERMINAL_home();
  record->splash(&TERMINAL);
  TERMINAL.print(record->getRunTime());
  TERMINAL.print(F(" REC"));
  TERMINAL.print(buffer.size());
  TERMINAL.print(F("\n"));

  if (recording) {
    buffer.unshift(record);               // buffer new record
    if (buffer.isFull()) { sd_dump(); }   // dump buffer to CSV file
  } else {
    delete record;                        // record from buffer
  }
}

void sd_dump(){
  if (buffer.isEmpty()) { return; }       // nothing to write

  rtc_now();    // new date for filename
  bool newfile = !SD.exists(FILENAME);
  CSV = SD.open(FILENAME, FILE_WRITE);
  if (!CSV) { SD.initErrorHalt(&TERMINAL); } // errorcode/message to TERMINAL
  if (newfile){
    buffer.first()->header(&CSV);
  }
  while (!buffer.isEmpty()) {
    buffer.first()->print(&CSV);
    delete buffer.shift();
  }
  CSV.close();
}

void recording_toggle(){
  if (recording) { sd_dump(); }           // dump buffer to SD card
  recording = not recording;              // pause/resume buffering
  TERMINAL_clear();
  TERMINAL.println((recording)?F("SD resumed"):F("SD paused"));
}

void safe_shutdown(){
  TERMINAL_clear();
  TERMINAL.println(F("Safe shutdown"));
  sd_dump();                              // dump buffer to SD card
  recording = false;                      // pause buffering
#ifdef HAS_SOFTPOWER
  TERMINAL.println(F("Powering down"));
  pinMode(BUTTON_PIN, OUTPUT);
  digitalWrite(BUTTON_PIN, LOW);         // trigger shutdown circuitry
#else
  TERMINAL.println(F("Remove power"));
#endif
  while (true) { delay(100); }
}
