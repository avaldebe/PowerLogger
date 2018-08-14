/*
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
*/

#include <Arduino.h>
#include "config.h"                       // project configuration
#include "Record.h"                       // secret sauce ;-)

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

bool recording = false;                   // off when starting
void recording_toggle();
void safe_shutdown();

void setup() {
  TERMINAL_begin();                       // start TERMINAL
  rtc_init(&TERMINAL);                    // update RTC if needed

  if (!SD.begin(chipSelect, SPI_SPEED)) {
    SD.initErrorHalt(&TERMINAL);          // errorcode/message to TERMINAL
  }

  Record::init(&TERMINAL, FILENAME);      // init/config INA devices

  button.setClickTicks(SHORTPRESS);       // single press duration [ms]
  button.attachClick(recording_toggle);   // pause/resume buffering
  button.setPressTicks(LONGPRESS);        // long press duration [ms]
  button.attachPress(safe_shutdown);      // dump buffen and power down
  button.attachDoubleClick(TERMINAL_toggle);  // switch backlight/display on/off
}

void loop() {
  static uint32_t last = 0;
  if (millis()-last<FREQUENCY) {
    // check for button presses untill is time for a new Record
    button.tick();
    delay(10);
    return;
  }
  last = millis();

  // measurements from all INA devices
  Record* record = new Record(last);
  record->splash(&TERMINAL);

  if (recording) {
    buffer.unshift(record);               // buffer new record
    if (buffer.isFull()) { sd_dump(); }   // dump buffer to CSV file
  } else {
    TERMINAL.println(F("Short press to resume SD recording"));
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
    buffer.shift()->print(&CSV);
  }
  CSV.close();
}

void recording_toggle(){
  if (recording) { sd_dump(); }           // dump buffer to SD card
  recording = not recording;              // pause/resume buffering
  TERMINAL.print(F("SD recording "));
  TERMINAL.println((recording)?F("resumed"):F("paused"));
}

void safe_shutdown(){
  TERMINAL.println(F("Safe shutdown started"));
  sd_dump();                              // dump buffer to SD card
  recording = false;                      // pause buffering
#ifdef HAS_SOFTPOWER
  TERMINAL.println(F("Powering down"));
  pinMode(BUTTON_PIN, OUTPUT);
  digitalWrite(BUTTON_PIN, LOW);         // trigger shutdown circuitry
#else
  TERMINAL.println(F("You can now safely remove power"));
#endif
}
