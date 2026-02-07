#include "timers.h"
#include "constants.h"
#include "data.h"
#include "lcd.h"
#include <Arduino.h>
#include <esp_timer.h>
#include "esp32/rom/ets_sys.h"

static esp_timer_handle_t timer1 = nullptr;
static esp_timer_handle_t timer2 = nullptr;

void IRAM_ATTR onTimer1(void* arg);
void IRAM_ATTR onTimer2(void* arg);

void setupTimers() {
  if (!timer1) {
    esp_timer_create_args_t t1 = { .callback = &onTimer1, .arg = nullptr, .dispatch_method = ESP_TIMER_TASK, .name = "t1" };
    esp_timer_create(&t1, &timer1);
  }
  if (!timer2) {
    esp_timer_create_args_t t2 = { .callback = &onTimer2, .arg = nullptr, .dispatch_method = ESP_TIMER_TASK, .name = "t2" };
    esp_timer_create(&t2, &timer2);
  }
}

void engageISR1() { if (timer1 && note1->period && note1->on_time) { esp_timer_stop(timer1); esp_timer_start_periodic(timer1, note1->period); } }
void disengageISR1(){ if (timer1) esp_timer_stop(timer1); }
void engageISR2() { if (timer2 && note2->period && note2->on_time) { esp_timer_stop(timer2); esp_timer_start_periodic(timer2, note2->period); } }
void disengageISR2(){ if (timer2) esp_timer_stop(timer2); }

void startTimer1() { engageISR1(); }
void startTimer2() { engageISR2(); }

void setTimer1f(unsigned int fr) {
  if (!timer1 || fr == 0 || note1->on_time == 0) { disengageISR1(); return; }
  note1->period = 1000000UL / fr;
  engageISR1();
}

void IRAM_ATTR onTimer1(void*) { ISR_backend(note1); }
void IRAM_ATTR onTimer2(void*) { ISR_backend(note2); }

void IRAM_ATTR ISR_backend(note* n) {
  long val = (volindex << 4);
  long tmp = val * (long)(n->on_time);
  int t = (int)(tmp >> 8);
  tmp = (long)(t) * (long)(n->velocity);
  t = (int)(tmp >> 8);
  if (t < MIN_ON_TIME) t = MIN_ON_TIME;
  digitalWrite(OPTICAL_PIN, HIGH);
  ets_delay_us(t);
  digitalWrite(OPTICAL_PIN, LOW);
  ets_delay_us(t);
}
