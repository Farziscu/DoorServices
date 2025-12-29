#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_sleep.h"
#include "esp_log.h"

#include "timers.h"

#define TIMER_WAKEUP_TIME_US (TIMER_WAKEUP_TIME_SEC * 1000 * 1000)
#define TIMER_START_SLEEP_MS (TIMER_START_SLEEP_SEC * 1000)

extern TaskHandle_t sleep_task_handle;

TimerHandle_t sleep_timer;

static int64_t last_read;
static esp_timer_handle_t periodic_timer;
extern SemaphoreHandle_t sem_temperature;

static const char *TAG = "timers";

static void timer_callback(void *arg);

/**
 * CallBack - notifies sleep_task to sleep
 */
void timers_sleep_cb(TimerHandle_t xTimer)
{
    ESP_LOGD(TAG, "timers_sleep_cb");
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(sleep_task_handle, 0, eNoAction, &xHigherPriorityTaskWoken);
}

/**
 * set the timer wake up after TIMER_WAKEUP_TIME_US seconds
 */
void timers_wakeup_set(void)
{
    ESP_LOGD(TAG, "timers_wakeup_set");

    uint64_t delta_us = TIMER_WAKEUP_TIME_US - (esp_timer_get_time() - last_read);

    // ESP_LOGD(TAG, "timers_wakeup_set delta_ms == %lld", delta_us);

    esp_sleep_enable_timer_wakeup(delta_us);
}

void timers_set_temperature_last_read(void)
{
    /* take care, evaluate race condition */
    last_read = esp_timer_get_time();
}

void timers_init(void)
{
    ESP_LOGD(TAG, "timers_init");
    sleep_timer = xTimerCreate("sleep_timer",
                               pdMS_TO_TICKS(TIMER_START_SLEEP_MS),
                               pdFALSE, // one-shot
                               NULL,
                               timers_sleep_cb);

    /****************/
    esp_timer_create_args_t args = {
        .callback = timer_callback,
        .name = "30s_timer"};

    ESP_ERROR_CHECK(esp_timer_create(&args, &periodic_timer));
    esp_timer_start_periodic(periodic_timer, 30ULL * 1000000ULL);
}

void timers_start(void)
{
    ESP_LOGI(TAG, "timers_start");
    xTimerStart(sleep_timer, 0);
}

static void timer_callback(void *arg)
{
    ESP_LOGI(TAG, "timer_callback");
    // risveglia il task
    xSemaphoreGive(sem_temperature);
}
