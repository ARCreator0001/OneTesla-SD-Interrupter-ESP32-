#include "timers.h"
#include "constants.h"
#include "data.h"
#include "lcd.h"
#include <Arduino.h>
#include <esp_timer.h>
#include "esp32/rom/ets_sys.h"

static esp_timer_handle_t timer1 = nullptr;
static esp_timer_handle_t timer2 = nullptr;
static esp_timer_handle_t timer3 = nullptr;
static esp_timer_handle_t timer4 = nullptr;

void IRAM_ATTR onTimer1(void* arg);
void IRAM_ATTR onTimer2(void* arg);
void IRAM_ATTR onTimer3(void* arg);
void IRAM_ATTR onTimer4(void* arg);

static inline void setVoiceTimer(esp_timer_handle_t timer, note* n) {
  if (!timer || !n || n->pitch == 0 || n->period == 0 || n->on_time == 0) {
    if (timer) esp_timer_stop(timer);
    return;
  }

  esp_timer_stop(timer);
  esp_timer_start_periodic(timer, n->period);
}

void setupTimers() {
  if (!timer1) {
    esp_timer_create_args_t t1 = { .callback = &onTimer1, .arg = nullptr, .dispatch_method = ESP_TIMER_TASK, .name = "t1" };
    esp_timer_create(&t1, &timer1);
  }
  if (!timer2) {
    esp_timer_create_args_t t2 = { .callback = &onTimer2, .arg = nullptr, .dispatch_method = ESP_TIMER_TASK, .name = "t2" };
    esp_timer_create(&t2, &timer2);
  }
  if (!timer3) {
    esp_timer_create_args_t t3 = { .callback = &onTimer3, .arg = nullptr, .dispatch_method = ESP_TIMER_TASK, .name = "t3" };
    esp_timer_create(&t3, &timer3);
  }
  if (!timer4) {
    esp_timer_create_args_t t4 = { .callback = &onTimer4, .arg = nullptr, .dispatch_method = ESP_TIMER_TASK, .name = "t4" };
    esp_timer_create(&t4, &timer4);
  }
}

void engageISR1() { setVoiceTimer(timer1, note1); }
void disengageISR1(){ if (timer1) esp_timer_stop(timer1); }
void engageISR2() { setVoiceTimer(timer2, note2); }
void disengageISR2(){ if (timer2) esp_timer_stop(timer2); }

void startTimer1() { engageISR1(); }
void startTimer2() { engageISR2(); }

void updateVoiceTimers() {
  setVoiceTimer(timer1, note1);
  setVoiceTimer(timer2, note2);
  setVoiceTimer(timer3, note3);
  setVoiceTimer(timer4, note4);
}

void setTimer1f(unsigned int fr) {
  if (!timer1 || fr == 0 || note1->on_time == 0) { disengageISR1(); return; }
  note1->period = 1000000UL / fr;
  engageISR1();
}

void IRAM_ATTR onTimer1(void*) { ISR_backend(note1); }
void IRAM_ATTR onTimer2(void*) { ISR_backend(note2); }
void IRAM_ATTR onTimer3(void*) { ISR_backend(note3); }
void IRAM_ATTR onTimer4(void*) { ISR_backend(note4); }

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
