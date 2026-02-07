#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <Arduino.h>

void setupADC(void);
void setupTimers(void);
void setupPins(void);
byte readFreqPot();
byte readPWPot();

#define ledOn()  digitalWrite(OPTICAL_PIN, HIGH)
#define ledOff() digitalWrite(OPTICAL_PIN, LOW)

#endif
