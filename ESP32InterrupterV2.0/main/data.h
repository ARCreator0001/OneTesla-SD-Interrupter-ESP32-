#ifndef __DATA_H
#define __DATA_H
#include "datatypes.h"
#include <LiquidCrystal_I2C.h>

extern note *note1, *note2, *note3, *note4;
extern midiMsg *last_message;
extern serialsource *serial;
extern sdsource *sd;
void blesource_init();
void blesource_run();
extern LiquidCrystal_I2C lcd;
extern char volindex, menuindex;

#endif
