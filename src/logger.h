#include <SdFat.h>

void LOGbegin();                         // INA.begin ...
void LOGheader(ArduinoOutStream *cout);   // stream header
bool LOGsave();                          // buffer measurement in EEPROM
void LOGdump(ArduinoOutStream *cout);     // stream buffered measuremnts
