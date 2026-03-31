#ifndef __PLAYER_H
#define __PLAYER_H
#include "datatypes.h"

extern bool allow_all_midi_channels;
extern bool midi_relaxed_poly;
extern volatile bool midi_stop;
extern volatile int midi_skip_dir;


void player_init();
void parsemsg(midiMsg *m);
void retrigger();

void note_on(unsigned char pitch, unsigned char velocity);
void note_off(unsigned char pitch);
void note_stop();

void control_change(unsigned char cmd, unsigned char value);
void pitch_bend(int value);
void set_polyphony_limit(uint8_t voices);
uint8_t get_polyphony_limit();

#endif
