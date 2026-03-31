#include <Arduino.h>
#include <SPI.h>
#include "constants.h"          // for SD_CS_PIN

// Tell MD_MIDIFile to use SdFat V2 FsFile types (avoids plain File typedef)
#define SD_FAT_TYPE 3
#include <SdFat.h>
#include <MD_MIDIFile.h>
#include "midisource.h"
#include "player.h"
#include "util.h"
#include "data.h"               // lcd, volindex
#include "lcd.h"

extern bool allow_all_midi_channels;
extern bool midi_relaxed_poly;

static SdFat       sdFat;
static MD_MIDIFile midi;

// ---- MIDI config knobs (easy to tweak) ----------------
// Track mask: 0xFFFFFFFF = all tracks; 0x3 = first two tracks only.
static constexpr uint32_t MIDI_TRACK_MASK = 0xFFFFFFFF;
// Channel filter: true = play all channels; false = only ch 0/1 (OMD-like).
static constexpr bool MIDI_ALLOW_ALL_CHANNELS = true;
// MIDI channel 10 (index 9) is percussion in GM; ignore it for cleaner coil output.
static constexpr bool MIDI_IGNORE_DRUMS_CH10 = true;
static constexpr uint8_t PRIORITY_CH_A = 0;  // MIDI ch 1
static constexpr uint8_t PRIORITY_CH_B = 1;  // MIDI ch 2
static constexpr uint8_t NON_PRIORITY_GAIN_NUM = 1;
static constexpr uint8_t NON_PRIORITY_GAIN_DEN = 2;
// -------------------------------------------------------

// Convert MD_MIDIFile event into your existing midiMsg/parsemsg
static void midiHandler(midi_event *pev) {
  const uint8_t ch = pev->channel & 0x0F;
  const uint8_t status = pev->data[0] & 0xF0;

  if (MIDI_IGNORE_DRUMS_CH10 && ch == 9 && (status == 0x80 || status == 0x90)) {
    return;
  }

  midiMsg m;
  m.cmd = pev->data[0] | ch;
  m.db1 = (pev->size > 1) ? pev->data[1] : 0;
  m.db2 = (pev->size > 2) ? pev->data[2] : 0;

  if (status == 0x90 && m.db2 > 0 && ch != PRIORITY_CH_A && ch != PRIORITY_CH_B) {
    uint16_t v = (uint16_t)m.db2 * NON_PRIORITY_GAIN_NUM / NON_PRIORITY_GAIN_DEN;
    m.db2 = (v == 0) ? 1 : (uint8_t)v;
  }

  parsemsg(&m);
}

void midisource_play(const char* path, const char* title) {
  uint8_t prevPoly = get_polyphony_limit();
  set_polyphony_limit(4);
  allow_all_midi_channels = MIDI_ALLOW_ALL_CHANNELS;
  midi_relaxed_poly       = true;
  midi_stop               = false;
  midi_skip_dir           = 0;   // unused for now (manual navigation after stop)

  SPI.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  if (!sdFat.begin(SD_CS_PIN, SPI_HALF_SPEED)) { note_stop(); midi_relaxed_poly = false; allow_all_midi_channels = false; set_polyphony_limit(prevPoly); return; }

  midi.begin(&sdFat);
  midi.setMidiHandler(midiHandler);
  midi.looping(false);
  // Track mask unsupported in this MD_MIDIFile version; to reduce tracks,
  // edit MIDI_MAX_TRACKS in MD_MIDIFile.h or preprocess the MIDI.

  if (midi.load(path) != MD_MIDIFile::E_OK) { note_stop(); midi_relaxed_poly = false; allow_all_midi_channels = false; set_polyphony_limit(prevPoly); return; }

  // Match OMD playback UI: status on line 0, volume bar on line 1
  (void)title;
  lcd_printhome("\2 ");
  for (int i = 0; i < volindex; i++) lcd_printat(i, 1, (char)(1));
  unsigned long st = millis(), ct = 0;
  unsigned long kt = 0;
  unsigned char prevKey = btnNONE;

  while (!midi.isEOF() && !midi_stop) {
    midi.getNextEvent();   // timing handled inside the library

    // poll keys for stop/volume adjustments
    unsigned long t = millis();
    if (t - kt > 50) {
      kt = t;
      unsigned char key = get_key();
      if (key != prevKey) {
        prevKey = key;
        if (key == btnSELECT) { midi_stop = true; note_stop(); break; }
        else if (key == btnBACK) { midi_stop = true; midi_skip_dir = 1; note_stop(); break; }
        else if (key == btnUP)   { incvol(&lcd); }
        else if (key == btnDOWN) { decvol(&lcd); }
      }
    }

    if (t - ct > 1000) {
      unsigned int tsec = (t - st) / 1000;
      lcd_printat(2, 0, 13, tsec / 60);
      lcd_print(':');
      unsigned int tmp = tsec % 60; if (tmp < 10) lcd_print('0'); lcd_print(tmp);
      lcd_print('/');
      lcd_print("--:--");
      ct = t;
    }
  }

done:
  midi.close();
  midi_relaxed_poly        = false;
  allow_all_midi_channels  = false;
  set_polyphony_limit(prevPoly);
}
