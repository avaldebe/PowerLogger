#include "INAbufer.h" // secret sauce ;-)
CircularBuffer<Record *, BUFFER_LEN> buffer;
static char linebuffer[16]; // long enough for 1 line

#include <INA.h>
INA_Class INA;
uint8_t Record::ina_count = 0; // no devices found so far

Record::Record(uint32_t time) : time(time)
{
  milliVolts = new uint32_t[ina_count];
  microAmps = new uint32_t[ina_count];
  for (uint8_t i = 0; i < ina_count; i++)
  {
    milliVolts[i] = INA.getBusMilliVolts(i);
    microAmps[i] = INA.getBusMicroAmps(i);
  }
}

char *Record::getRunTime(uint32_t s)
{
  uint8_t d, h, m;
  d = s / 86400;
  s %= 86400; // 86400 secs on a day
  h = s / 3600;
  s %= 3600; //  3600 secs on a hour
  m = s / 60;
  s %= 60; //    60 secs on a minute
  sprintf(linebuffer, "%02u:%02u:%02u:%02u", d, h, m, (uint8_t)s);
  return linebuffer;
}

uint8_t Record::init(uint8_t maxBusAmps, uint32_t microOhmR)
{
  ina_count = INA.begin(maxBusAmps, microOhmR);
  INA.setBusConversion(INA_CONVTIME);    // see config.h for value
  INA.setShuntConversion(INA_CONVTIME);  // see config.h for value
  INA.setAveraging(INA_SAMPLES);         // see config.h for value
  INA.setMode(INA_MODE_CONTINUOUS_BOTH); // bus&shunt
  return ina_count;
}

uint8_t Record::init(Print *out)
{
  init();
  while (ina_count == 0)
  {
    out->print(F("ERROR: no INA devices found"));
    delay(1000);
    init(); // try again
  }

  for (uint8_t i = 0; i < ina_count; i++)
  {
    out->print(F("INA"));
    out->print(i);
    out->print(F(": "));
    out->println(INA.getDeviceName(i));
  }

  out->print(F("Buffer "));
  out->print(size());
  out->print(F("Bx"));
  out->print(max_len());
  out->println("rec");

  return ina_count;
}

void Record::header(Print *out)
{
  out->print(F("millis"));
  for (uint8_t i = 0; i < ina_count; i++)
  {
    out->print(F(",ch"));
    out->print(i);
    out->print(F(" voltage [mV]"));
    out->print(F(",ch"));
    out->print(i);
    out->print(F(" current [uA]"));
  }
  out->println();
}

void Record::print(Print *out)
{
  out->print(time);
  for (uint8_t i = 0; i < ina_count; i++)
  {
    out->print(F(","));
    out->print(milliVolts[i]);
    out->print(F(","));
    out->print(microAmps[i]);
  }
  out->println();
}

void Record::splash(Print *out, uint8_t width, uint8_t height)
{
  bool header;
  uint8_t i;
  switch (width)
  {
  case 16 ... 255: // wide screen. eg 128x64 or 128x32
    header = height > ina_count;
    //                          0123456789ABCDEF
    if (header)
    {
      out->print(F("#   V [V]  I [A]\n"));
    }
    //                         "1: 23.000  1.000"
    for (i = 0; i < ina_count; i++)
    {
      out->print(i);
      out->print(F(":"));
      out->print(dtostrf(getVolts(i), 7, 3, linebuffer));
      out->print(dtostrf(getAmps(i), 7, 3, linebuffer));
      out->print(F("\n"));
    }
    break;
  case 12 ... 15: // narrow screen, eg 98X68
    header = height > ina_count;
    //                          0123456789ABC
    if (header)
    {
      out->print(F("# V [V] I [A]\n"));
    }
    //                         "1 23.00 1.000"
    for (i = 0; i < ina_count; i++)
    {
      out->print(i);
      out->print(dtostrf(getVolts(i), 6, 2, linebuffer));
      out->print(dtostrf(getAmps(i), 6, 3, linebuffer));
      out->print(F("\n"));
    }
    break;
  case 6 ... 11: // narrow screen, eg 84X48 or 64X48
    header = width > 7;
    for (i = 0; i < ina_count; i++)
    {
      //                          012345678
      if (header)
      {
        out->print(F("V"));
      }
      //                         "V1 23.000"
      out->print(i);
      out->print(dtostrf(getVolts(i), 7, 3, linebuffer));
      out->print(F("\n"));
      //                          012345678
      if (header)
      {
        out->print(F("A"));
      }
      //                         "A1  1.000"
      out->print(i);
      out->print(dtostrf(getAmps(i), 7, 3, linebuffer));
      out->print(F("\n"));
    }
    break;
  }
}
