#ifndef __UTIL_H
#define __UTIL_H
#include "datatypes.h"
#include <LiquidCrystal_I2C.h>

//note handling
void note_kill(note *n);
void make_note(note *n, unsigned char pitch, unsigned char velocity);
//frequency utilities
unsigned int get_on_time(long freq);
unsigned long get_freq(unsigned char note);
//keypresses
unsigned char get_key();
//volume bar
void incvol(LiquidCrystal_I2C *lcd);
void decvol(LiquidCrystal_I2C *lcd);
//file utils
unsigned char chk_ext(char* fname);
//some debugging utilities
void print_hex_buf(LiquidCrystal_I2C *lcd, unsigned char* buf, unsigned char count);
void print_tmsg(LiquidCrystal_I2C *lcd, timedMidiMsg *m);
void print_byte(LiquidCrystal_I2C *lcd, unsigned char x);

#endif
