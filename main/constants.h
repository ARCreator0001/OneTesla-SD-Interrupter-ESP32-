/*
 * constants.h
 * Contains defined constants, user-modifiable macros, and structures
 * Read the manual to see what to modify
 */
#ifndef __CONSTANTS_H
#define __CONSTANTS_H

#include "system.h"

//#define DEBUG

/****Coil-dependent configuration variables****/
#define LOOKUP_TABLE_SCALE 3
#define MIN_ON_TIME 10
#define ON_TIME_ARRAY_LENGTH 26
static byte on_times[] =
{33,33,33,33,27,23,20,20,18,17,16,15,14,13,12,11,10,9,8,8,8,8,8,8,8,8};

/****Instrument-dependent configuration variables****/
#define SUSTAIN_TIME 750000
#define SUSTAIN_TIME_OVER_256 2930
#define map_velocity(v) (127 + v / 2)
#define map_sustain(t) ((long) (t) / SUSTAIN_TIME_OVER_256)

/****System stuff****/
#define FILE_BUFFER_SIZE 256
#define TIMER_2_PERIOD_MAX 16000  // used in player.cpp

// ESP32 pins
#define SD_CS_PIN    5
#define OPTICAL_PIN  25
#define LCD_I2C_ADDR 0x27
#define BTN_UP       32
#define BTN_DN       33
#define BTN_OK       27
#define MIDI_TX_PIN  17
#define MIDI_RX_PIN  16

static unsigned int base[] = {220,233,247,262,277,294,311,330,349,370,392,415};
static unsigned char pow2[] = {1,2,4,8,16,32};
static char chex[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
static unsigned char block[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
static unsigned char arrow[8] = {0xF0,0xF8,0xFC,0xFE,0xFC,0xF8,0xF0,0x00};



#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define MENU_SDCARD 0
#define MENU_FIXED  2
#define MENU_LIVE   1

#endif
