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

- BlinkMachine example from the OneButton library
  https://github.com/mathertel/OneButton

- Unit Testing of a “Blink” Project
  http://docs.platformio.org/en/latest/tutorials/core/unit_testing_blink.html

- STM32F103 low power settings
  http://github.com/cbm80amiga/N5110_STM32_power_consumption
*/

#include <Arduino.h>
#include "config.h"   // project configuration
#include <INAbufer.h> // secret sauce ;-)

#include <SdFat.h>
SdFat SD; // File system object.
File CSV; // File object for filename

#include <RTCutil.h>
#ifdef HAST_RTC
#define FILENAME rtc_fmt('C') // 'YYMMDD.csv'
#else
#define FILENAME "INA.csv"
#endif

#include <TERMutil.h>
void sd_dump();

#include <OneButton.h>
#ifndef BUTTON_PIN
#error "Undefined BUTTON_PIN flag. Try with -D BUTTON_PIN=3"
#endif
OneButton button(BUTTON_PIN, true); // with INPUT_PULLUP

bool recording = false; // write to SD (or not)
void recording_toggle();
void safe_shutdown();
void backlight_toggle();

#ifdef __STM32F1__
#include <libmaple/pwr.h>
#include <libmaple/scb.h>
#endif
void low_power();

void setup()
{
  low_power();
#ifdef LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT); // blink LED with activity
#endif
  rtc_init(); // set RTC if needed
#ifdef SHORTPRESS
  button.setClickTicks(SHORTPRESS); // single press duration [ms]
#endif
#ifdef LONGPRESS
  button.setPressTicks(LONGPRESS); // long press duration [ms]
#endif
  button.attachClick(recording_toggle); // pause/resume buffering
  button.attachPress(safe_shutdown);    // dump buffen and power down
#ifdef BACKLIGHT_PIN
  button.attachDoubleClick(backlight_toggle); // switch backlight on/off
  // display backlight attached/controlled by BACKLIGHT_PIN
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, LOW); // backlight off
#else
  button.attachDoubleClick(TERMINAL_toggle); // switch display on/off
#endif

  TERMINAL_begin(); // start TERMINAL

  // 1st splash page
  TERMINAL.println(F("PowerLogger"));
#ifdef GIT_REV
  TERMINAL.print(F("\nRev:\n" GIT_REV "\n"));
#endif
  TERMINAL.print(F("\nBuild:\n" __DATE__ "\n" __TIME__ "\n"));
  TERMINAL_clear(2000); // wait 2 sec before continuing

  // 2nd splash page
  TERMINAL.print(F("RTC @"));
  TERMINAL.println(rtc_now());

  Record::init(&TERMINAL); // init/config INA devices
  if (!SD.begin(SD_CS, SPI_SPEED))
  {
    SD.initErrorHalt(&TERMINAL); // errorcode/message to TERMINAL
  }
  TERMINAL.print(F("SD: "));
  TERMINAL.println(FILENAME);
  if (!recording)
  { // wait until SD is resumed
    TERMINAL.println(F("SD: paused"));
  }
  TERMINAL_clear(2000); // wait 2 sec before continuing
}

void loop()
{
#ifdef LED_BUILTIN
  digitalWrite(LED_BUILTIN, LOW); // blink LED with activity
#endif
  static uint32_t last = 0;
  do
  { // test at least once
    // check for button presses until is time for a new Record
    button.tick();
    delay(10);
  } while (millis() - last < FREQUENCY);
  last = millis();
#ifdef LED_BUILTIN
  digitalWrite(LED_BUILTIN, HIGH); // blink LED with activity
#endif

  // measurements from all INA devices
  Record *record = new Record(last);
  if (!record)
  {
    TERMINAL.println(F("Out of memory"));
    return;
  }
  TERMINAL_home();
  record->splash(&TERMINAL, display.cols, display.rows);
  TERMINAL.print(record->getRunTime());
  TERMINAL.print(recording ? F(" R:") : F(" P:"));
  TERMINAL.print(buffer.size());
  TERMINAL.print(F(" \n"));

  if (recording)
  {
    buffer.unshift(record); // buffer new record
    if (buffer_full())
    {
      sd_dump();
    } // dump buffer to CSV file
  }
  else
  {
    delete record; // record from buffer
  }
}

void sd_dump()
{
  if (buffer.isEmpty())
  {
    return;
  } // nothing to write

  rtc_now(); // new date for filename
  bool newfile = !SD.exists(FILENAME);
  CSV = SD.open(FILENAME, FILE_WRITE);
  if (!CSV)
  {
    SD.initErrorHalt(&TERMINAL);
  } // errorcode/message to TERMINAL
  if (newfile)
  {
    buffer.first()->header(&CSV);
  }
  while (!buffer.isEmpty())
  {
    buffer.first()->print(&CSV);
    delete buffer.shift();
  }
  CSV.close();
}

void recording_toggle()
{
  if (recording)
  {
    sd_dump();
  }                          // dump buffer to SD card
  recording = not recording; // pause/resume buffering
}

void safe_shutdown()
{
  TERMINAL_clear();
  TERMINAL.println(F("Safe shutdown"));
  sd_dump();         // dump buffer to SD card
  recording = false; // pause buffering
#ifdef HAS_SOFTPOWER
  TERMINAL.println(F("Powering down"));
  pinMode(BUTTON_PIN, OUTPUT);
  digitalWrite(BUTTON_PIN, LOW); // trigger shutdown circuitry
#else
  TERMINAL.println(F("Remove power"));
#endif
  while (true)
  {
    delay(100);
  }
}

void backlight_toggle()
{
#ifdef BACKLIGHT_PIN
  bool is_on = digitalRead(BACKLIGHT_PIN); // backlight on?
  is_on ^= true;                           // same as !is_on
  // display backlight attached/controlled by BACKLIGHT_PIN
  digitalWrite(BACKLIGHT_PIN, is_on ? HIGH : LOW);
#endif
}

void low_power()
{
#ifdef __STM32F1__
  // unused ADCs
  adc_disable_all();
  // unused clocks
  rcc_clk_disable(RCC_ADC1);
  rcc_clk_disable(RCC_ADC2);
  rcc_clk_disable(RCC_ADC3);
  rcc_clk_disable(RCC_AFIO);
  //rcc_clk_disable(RCC_BKP);  // internal RTC(?)
  rcc_clk_disable(RCC_CRC);
  rcc_clk_disable(RCC_DAC);
  rcc_clk_disable(RCC_DMA1);
  rcc_clk_disable(RCC_DMA2);
  rcc_clk_disable(RCC_FLITF);
  rcc_clk_disable(RCC_FSMC);
  //rcc_clk_disable(RCC_GPIOA);
  //rcc_clk_disable(RCC_GPIOB);
  //rcc_clk_disable(RCC_GPIOC);
  rcc_clk_disable(RCC_GPIOD);
  rcc_clk_disable(RCC_GPIOE);
  rcc_clk_disable(RCC_GPIOF);
  rcc_clk_disable(RCC_GPIOG);
  //rcc_clk_disable(RCC_I2C1); // TERMINAL, external RTC
  rcc_clk_disable(RCC_I2C2);
  //rcc_clk_disable(RCC_PWR);  // internal RTC(?)
  rcc_clk_disable(RCC_SDIO);
  //rcc_clk_disable(RCC_SPI1); // TERMINAL, SD card
#ifndef DISPLAY_H2_SPI
  rcc_clk_disable(RCC_SPI2); // TERMINAL
#endif
  rcc_clk_disable(RCC_SPI3);
  rcc_clk_disable(RCC_SRAM);
  rcc_clk_disable(RCC_TIMER1);
  rcc_clk_disable(RCC_TIMER2);
  rcc_clk_disable(RCC_TIMER3);
  rcc_clk_disable(RCC_TIMER4);
  rcc_clk_disable(RCC_TIMER5);
  rcc_clk_disable(RCC_TIMER6);
  rcc_clk_disable(RCC_TIMER7);
  rcc_clk_disable(RCC_TIMER8);
  rcc_clk_disable(RCC_TIMER9);
  rcc_clk_disable(RCC_TIMER10);
  rcc_clk_disable(RCC_TIMER11);
  rcc_clk_disable(RCC_TIMER12);
  rcc_clk_disable(RCC_TIMER13);
  rcc_clk_disable(RCC_TIMER14);
#ifdef TERM_U8X8
  rcc_clk_disable(RCC_USART1); // TERMINAL
  rcc_clk_disable(RCC_USART2); // TERMINAL
  rcc_clk_disable(RCC_USART3); // TERMINAL
#endif
  rcc_clk_disable(RCC_UART4);
  rcc_clk_disable(RCC_UART5);
  rcc_clk_disable(RCC_USB);
#endif
}
