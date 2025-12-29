#pragma once

#include "freertos/timers.h"
#include "esp_err.h"

#define TIMER_WAKEUP_TIME_SEC (30)                                 // wakeup after this time
#define TIMER_WAKEUP_TIME_US (TIMER_WAKEUP_TIME_SEC * 1000 * 1000) // wakeup after this time
#define TIMER_START_SLEEP_SEC (5)                                  // sec  ; go to sleep after this time of inactivity

void timers_sleep_cb(TimerHandle_t xTimer);
void timers_wakeup_set(void);
void timers_set_temperature_last_read(void);
void timers_init(void);
void timers_start(void);
