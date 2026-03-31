#include "player.h"
#include "util.h"
#include "data.h"
#include "system.h"
#include "timers.h"
#include "constants.h"
#include "lcd.h"
#include <stdlib.h>
#include <string.h>

bool allow_all_midi_channels = false;
bool midi_relaxed_poly = false;
volatile bool midi_stop = false;
volatile int midi_skip_dir = 0;

static uint8_t polyphonyLimit = 2;

static inline note* voiceAt(uint8_t i) {
  if (i == 0) return note1;
  if (i == 1) return note2;
  if (i == 2) return note3;
  return note4;
}

static inline uint8_t clampVoices(uint8_t voices) {
  if (voices < 1) return 1;
  if (voices > 4) return 4;
  return voices;
}

void set_polyphony_limit(uint8_t voices) {
  polyphonyLimit = clampVoices(voices);

  // Hard-stop voices outside current limit.
  for (uint8_t i = polyphonyLimit; i < 4; i++) {
    voiceAt(i)->pitch = 0;
  }
  retrigger();
}

uint8_t get_polyphony_limit() {
  return polyphonyLimit;
}

void note_on(unsigned char pitch, unsigned char value) {
  if (value == 0) { note_off(pitch); return; }

  // Refresh existing voice if same pitch already active.
  for (uint8_t i = 0; i < polyphonyLimit; i++) {
    note* v = voiceAt(i);
    if (v->pitch == pitch) {
      make_note(v, pitch, value);
      retrigger();
      return;
    }
  }

  // Fill an empty slot first.
  for (uint8_t i = 0; i < polyphonyLimit; i++) {
    note* v = voiceAt(i);
    if (v->pitch == 0) {
      make_note(v, pitch, value);
      retrigger();
      return;
    }
  }

  // All slots full.
  if (midi_relaxed_poly) {
    // Replace weakest/lowest voice only when new note is stronger or higher.
    uint8_t victim = 0;
    for (uint8_t i = 1; i < polyphonyLimit; i++) {
      note* vi = voiceAt(i);
      note* vv = voiceAt(victim);
      if (vi->pitch < vv->pitch || (vi->pitch == vv->pitch && vi->velocity < vv->velocity)) {
        victim = i;
      }
    }

    note* low = voiceAt(victim);
    unsigned char mappedValue = map_velocity(value);
    if (mappedValue > low->velocity || pitch > low->pitch) {
      make_note(low, pitch, value);
      retrigger();
    }
    return;
  }

  // Legacy-style FIFO replace: shift older voices down, append new at tail.
  for (uint8_t i = 0; i + 1 < polyphonyLimit; i++) {
    memcpy(voiceAt(i), voiceAt(i + 1), sizeof(note));
  }
  make_note(voiceAt(polyphonyLimit - 1), pitch, value);
  retrigger();
}

void note_off(unsigned char pitch) {
  bool changed = false;
  for (uint8_t i = 0; i < polyphonyLimit; i++) {
    note* v = voiceAt(i);
    if (v->pitch == pitch) {
      v->pitch = 0;
      changed = true;
    }
  }
  if (changed) retrigger();
}

void note_stop() {
  for (uint8_t i = 0; i < 4; i++) {
    voiceAt(i)->pitch = 0;
  }
  retrigger();
}

void control_change(unsigned char cmd, unsigned char value) {
  // All notes/sound off
  if (cmd == 0x78 || cmd == 0x79 || cmd == 0x7B || cmd == 0x7C) { note_stop(); return; }
  // Sustain pedal release (legacy behavior)
  if (cmd == 0x40 && value < 64) { retrigger(); return; }
}

void retrigger() {
  updateVoiceTimers();
}

void parsemsg(midiMsg *m) {
  unsigned char channel = m->cmd & 0x0f;
  if (!allow_all_midi_channels) {
    if (channel != 1 && channel != 0) return;  // original filter
  }
  if (m->cmd == 0xfc) { note_stop(); return; }
  unsigned char cmd = (m->cmd & 0xf0) >> 4;
  if (cmd == 0x09) note_on(m->db1, m->db2);
  else if (cmd == 0x08) note_off(m->db1);
  else if (cmd == 0x0b) control_change(m->db1, m->db2);
}

void player_init() {
  note1 = (note*) malloc(sizeof(note));
  note2 = (note*) malloc(sizeof(note));
  note3 = (note*) malloc(sizeof(note));
  note4 = (note*) malloc(sizeof(note));

  note1->pitch = 0;
  note2->pitch = 0;
  note3->pitch = 0;
  note4->pitch = 0;
  polyphonyLimit = 2;
}
