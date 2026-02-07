#ifndef __DATA_H
#define __DATA_H
#include "datatypes.h"
#include <LiquidCrystal_I2C.h>

extern note *note1, *note2;
extern midiMsg *last_message;
extern serialsource *serial;
extern sdsource *sd;
extern LiquidCrystal_I2C lcd;
extern char volindex, menuindex;

#endif
