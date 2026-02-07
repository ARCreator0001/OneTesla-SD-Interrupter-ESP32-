/*
 * System dependent functions
 */
#include "system.h"
#include "constants.h"

void setupADC() {
  // No pots wired on the reference ESP32 build; stub for compatibility.
}

void setupPins() {
  pinMode(OPTICAL_PIN, OUTPUT);
  digitalWrite(OPTICAL_PIN, LOW);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DN, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
}

byte readFreqPot() { return 0; }
byte readPWPot()  { return 0; }
