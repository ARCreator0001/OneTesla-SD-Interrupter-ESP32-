#include "datatypes.h"
#include "data.h"
#include "util.h"
#include "constants.h"
#include "lcd.h"
#include <Arduino.h>

static int8_t read_encoder_step() {
  static bool initialized = false;
  static uint8_t lastState = 0;
  static int8_t accum = 0;
  static const int8_t transitionTable[16] = {
    0, -1,  1,  0,
    1,  0,  0, -1,
   -1,  0,  0,  1,
    0,  1, -1,  0
  };

  uint8_t state = (digitalRead(ENC_A_PIN) << 1) | digitalRead(ENC_B_PIN);
  if (!initialized) {
    initialized = true;
    lastState = state;
    return 0;
  }

  if (state == lastState) {
    return 0;
  }

  accum += transitionTable[(lastState << 2) | state];
  lastState = state;

  if (accum >= 4) {
    accum = 0;
    return 1;
  }
  if (accum <= -4) {
    accum = 0;
    return -1;
  }
  return 0;
}

void note_kill(note *n) { n->pitch = 0; }

void make_note(note *n, unsigned char pitch, unsigned char velocity) {
  n->pitch = pitch;
  unsigned long freq = get_freq(pitch);
  n->period = 1000000 / freq;
  n->on_time = get_on_time(freq);
  n->velocity = map_velocity(velocity);
}

unsigned long get_freq(unsigned char pitch) {
  int n = pitch - 57;
  if (n >= 0) return pow2[n / 12] * base[n % 12];
  int a = (-(n + 1)) / 12 + 1;
  int r = 12 * a + n;
  return base[r] / pow2[a];
}

unsigned int get_on_time(long freq) {
  int index = freq / 100;
  // Keep high notes audible: clamp index instead of returning 0.
  if (index < 0) index = 0;
  if (index > ON_TIME_ARRAY_LENGTH - 1) index = ON_TIME_ARRAY_LENGTH - 1;
  return LOOKUP_TABLE_SCALE * on_times[index];
}

unsigned char get_key() {
#ifndef DEBUG
 static unsigned long backLockoutUntil = 0;

 if (!digitalRead(ENC_SW_PIN)) {
   backLockoutUntil = millis() + 80;
   return btnSELECT;
 }
 if ((long)(millis() - backLockoutUntil) >= 0 && !digitalRead(BTN_BACK_PIN)) return btnBACK;
 if (!digitalRead(BTN_LEFT_PIN)) return btnUP;
 if (!digitalRead(BTN_RIGHT_PIN)) return btnDOWN;

 int8_t enc = read_encoder_step();
 if (enc > 0) return btnDOWN;
 if (enc < 0) return btnUP;

 return btnNONE;
#else
 int adc_key_in = analogRead(0);
 if (adc_key_in > 1000) return btnNONE;
 if (adc_key_in < 50)   return btnRIGHT;
 if (adc_key_in < 195)  return btnUP;
 if (adc_key_in < 380)  return btnDOWN;
 if (adc_key_in < 555)  return btnLEFT;
 if (adc_key_in < 790)  return btnSELECT;
 return btnNONE;
#endif
}

void incvol(LiquidCrystal_I2C *lcd) {
  if (volindex == 16) return;
  lcd_printat(volindex, 1, (char) (1));
  volindex++;
}

void decvol(LiquidCrystal_I2C *lcd) {
  if (volindex == 0) return;
  lcd_printat(volindex - 1, 1, ' ');
  volindex--;
}

void print_hex_buf(LiquidCrystal_I2C *lcd, unsigned char* buf, unsigned char count) {
  for (char i = 0; i < count; i++) {
    char h = (buf[i] & 0xf0) >> 4;
    char l = buf[i] & 0x0f;
    lcd->print(chex[h]); lcd->print(chex[l]); lcd->print(' ');
  }
}

void print_byte(LiquidCrystal_I2C *lcd, unsigned char x) {
  char h = (x & 0xf0) >> 4;
  char l = x & 0x0f;
  lcd->print(chex[h]); lcd->print(chex[l]); lcd->print(' ');
}

void print_tmsg(LiquidCrystal_I2C *lcd, timedMidiMsg *m) {
  lcd->print((int) (m->time)); lcd->print(" ");
  if (m->cmd == 0x80) lcd->print("Off ");
  if (m->cmd == 0x90) lcd->print("On ");
  print_byte(lcd, m->db1); print_byte(lcd, m->db2);
}
