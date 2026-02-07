#ifndef __TIMERS_H
#define __TIMERS_H

#include "datatypes.h"

void setupTimers();
void startTimer1();
void startTimer2();
void setTimer1f(unsigned int fr);
void engageISR1();
void disengageISR1();
void engageISR2();
void disengageISR2();
void ISR_backend(note* n);

#endif
