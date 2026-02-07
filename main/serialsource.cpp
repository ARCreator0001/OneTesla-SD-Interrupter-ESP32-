#include "serialsource.h"
#include "datatypes.h"
#include "data.h"
#include "player.h"
#include "util.h"
#include "constants.h"
#include "lcd.h"

#include <stdlib.h>
#include <HardwareSerial.h>

HardwareSerial &MIDI_SERIAL = Serial2;

void serialsource_init() {
  MIDI_SERIAL.begin(BAUDRATE, SERIAL_8N1, MIDI_RX_PIN, MIDI_TX_PIN);
  serial = (serialsource*) malloc(sizeof(serialsource));
  serial->last_status = 0;
  serial->buf = (unsigned char*) malloc(2);
  serial->remaining = 0;
  serial->total = 2;
  serial->buf_idx = 0;
  last_message = (midiMsg*) malloc(sizeof(midiMsg));
}

void serialsource_run() {
  note1->pitch = note2->pitch = 0;
  unsigned char b;
  unsigned long lt = millis();
  for (;;) {
    unsigned long t = millis();
    if (t - lt > 150) {
      lt = t;
      unsigned char k = get_key();
      if (k == btnSELECT) { note_stop(); return; }
      else if (k == btnUP)   incvol(&lcd);
      else if (k == btnDOWN) decvol(&lcd);
    }
    if (!MIDI_SERIAL.available()) continue;
    b = MIDI_SERIAL.read();

    if (b & 0x80) { // status
      unsigned char msn = b & 0xf0;
      if (msn == 0x80 || msn == 0x90 || msn == 0xb0 || msn == 0xe0) {
        serial->last_status = b;
        serial->remaining = 2;
        serial->buf_idx = 0;
      } else {
        serial->last_status = 0;
      }
      continue;
    }

    if (serial->last_status == 0) continue;
    serial->buf[serial->buf_idx++] = b;
    serial->remaining--;

    if (serial->remaining == 0) {
      last_message->cmd = serial->last_status;
      last_message->db1 = serial->buf[0];
      last_message->db2 = serial->buf[1];
      serial->remaining = serial->total;
      serial->buf_idx = 0;
      parsemsg(last_message);
    }
  }
}
