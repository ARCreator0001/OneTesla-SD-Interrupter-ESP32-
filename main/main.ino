/*
 * oneTesla SD interrupter (ESP32)
 */
#include "constants.h"
#include "data.h"
#include "datatypes.h"
#include "sdsource.h"
#include "serialsource.h"
#include "player.h"
#include "system.h"
#include "util.h"
#include "timers.h"
#include "lcd.h"

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(LCD_I2C_ADDR, 16, 2);

note *note1, *note2;
midiMsg *last_message;
serialsource *serial;
sdsource *sd;
char volindex = 0, menuindex = 2;
int ffreq = 20;

void fixedLoop();
void displayMenu();

void setup() {
  Wire.begin();
  lcd_init();
  setupTimers();
  setupPins();
  player_init();
  sdsource_init();
  serialsource_init();
  displayMenu();
}

void loop() {
  unsigned char key = get_key();
  if (key == btnDOWN) { menuindex = (menuindex == 2) ? 0 : menuindex + 1; displayMenu(); delay(180); }
  if (key == btnUP)   { menuindex = (menuindex == 0) ? 2 : menuindex - 1; displayMenu(); delay(180); }
  if (key == btnSELECT) {
    if (menuindex == MENU_FIXED) { delay(150); fixedLoop(); delay(300); }
    else if (menuindex == MENU_LIVE) {
      delay(150);
      lcd_printhome("Live Mode ON");
      lcd_setcursor(0, 1);
      for (int i = 0; i < volindex; i++) lcd_print((char)1);
      serialsource_run();
      lcd_printhome("Live Mode");
      delay(300);
    } else { // MENU_SDCARD
      delay(150);
      if (sd->valid) sdsource_run(); else sdsource_initcard();
      delay(300);
    }
  }
}

void displayMenu() {
  if (menuindex == 0) {
    lcd_printhome("SD Card");
    lcd_setcursor(0, 1);
    if (sd->valid) {
      lcd_print(sd->file_count);
      lcd_print(sd->file_count == 1 ? " file, " : " files, ");
      lcd_print(sd->dir_count);
      lcd_print(sd->dir_count == 1 ? " dir" : " dirs");
    } else {
      lcd_print(sd->last_error);
    }
  } else if (menuindex == 1) {
    lcd_printhome("Live Mode");
  } else {
    lcd_printhome("Fixed Mode");
  }
}

void fixedLoop() {
  lcd_printhome("[Freq: ");
  lcd_print(ffreq);
  lcd_print("Hz");
  lcd_printat(15, 0, ']');
  lcd_setcursor(0, 1);
  for (int i = 0; i < volindex; i++) lcd_print((char)1);
  char mode = 1;
  unsigned long elapsed = 0;
  unsigned int uinc, dinc = 1;
  unsigned long selectStart = 0;
  note1->velocity = 127;
  note1->on_time = get_on_time(ffreq);
  setTimer1f(ffreq);
  engageISR1();
  for (;;) {
    unsigned char key = get_key();
    // long-press SELECT (~1s) to exit Fixed Mode
    if (key == btnSELECT) {
      if (selectStart == 0) selectStart = millis();
      if (millis() - selectStart > 1000) {
        note_stop();
        disengageISR1();
        disengageISR2();
        lcd_printhome("Exit Fixed");
        delay(300);
        return;  // back to main menu
      }
    } else {
      selectStart = 0;
    }

    if (elapsed > 100000) {
      elapsed = 0;
      if (key != btnUP) uinc = 1;
      if (key != btnDOWN) dinc = 1;
      if (key == btnSELECT) {
        mode = !mode;
        if (mode) {
          lcd_printhome("[Freq: "); lcd_print(ffreq); lcd_print("Hz"); lcd_printat(15, 0, ']');
          lcd_setcursor(0, 1); for (int i = 0; i < volindex; i++) lcd_print((char)1);
        } else {
          lcd_printhome("Freq: "); lcd_print(ffreq); lcd_print("Hz");
          lcd_setcursor(0, 1); for (int i = 0; i < volindex; i++) lcd.print((char)1);
          lcd_printat(0, 1, '['); lcd_printat(15, 1, ']');
        }
      }
      if (key == btnUP) {
        if (!mode) { lcd_setcursor(0, 1); incvol(&lcd); lcd_printat(0, 1, '['); lcd_printat(15, 1, ']'); }
        else { ffreq += uinc; uinc += 5; if (ffreq > 1000) ffreq = 1000;
               lcd_printat(7, 0, 7, (unsigned int)ffreq); lcd_print("Hz");
               note1->on_time = get_on_time(ffreq); setTimer1f(ffreq); }
      }
      if (key == btnDOWN) {
        if (!mode) { lcd_setcursor(0, 1); decvol(&lcd); lcd_printat(0, 1, '['); lcd_printat(15, 1, ']'); }
        else { ffreq -= dinc; dinc += 5; if (ffreq < 1) ffreq = 1;
               lcd_printat(7, 0, 7, (unsigned int)ffreq); lcd_print("Hz");
               note1->on_time = get_on_time(ffreq); setTimer1f(ffreq); }
      }
    }
    delayMicroseconds(1000);
    elapsed += 1000;
  }
}
