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
  pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
  pinMode(BTN_BACK_PIN, INPUT_PULLUP);
  pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
  pinMode(ENC_A_PIN, INPUT_PULLUP);
  pinMode(ENC_B_PIN, INPUT_PULLUP);
  pinMode(ENC_SW_PIN, INPUT_PULLUP);
}

byte readFreqPot() { return 0; }
byte readPWPot()  { return 0; }
