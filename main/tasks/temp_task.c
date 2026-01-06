#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "drivers/temperature/temp_sensor.h"
#include "app/app_events.h"
#include "app/timers.h"

#define READ_TEMP_PERIOD_SEC TIMER_WAKEUP_TIME_SEC
#define READ_TEMP_PERIOD_MS (READ_TEMP_PERIOD_SEC * 1000)

extern QueueHandle_t queue_app_event;
extern SemaphoreHandle_t sem_temperature;

static const char *TAG = "temp_task";

void temp_task(void *arg)
{
    ESP_LOGI(TAG, "temp_task starts");

    // TickType_t last_wake = xTaskGetTickCount();
    // const TickType_t period = pdMS_TO_TICKS(READ_TEMP_PERIOD_MS);

    temp_sensor_init();
    // temperature_init();
    vTaskDelay(pdMS_TO_TICKS(50)); // wait to prepare sensor for the first read

    while (1)
    {
        temp_t temp_value = {0};

        app_event_t evt = {.id = EVT_TEMP_UPDATE};

        // BaseType_t ret = xSemaphoreTake(sem_temperature, pdMS_TO_TICKS(READ_TEMP_PERIOD_MS));
        BaseType_t ret = xSemaphoreTake(sem_temperature, portMAX_DELAY);

        ESP_LOGI(TAG, "temp_task - Got temp semaphore, ret == %d", ret);

        timers_set_temperature_last_read();

        // if (temperature_read(&temp_value) == TMP_OK)
        if (temperature_read(&temp_value) == true)
        {
            evt.data.tempSens.temperature = temp_value.temperature;
            evt.data.tempSens.pressure = temp_value.pressure;
        }
        else
        {
            evt.data.error_code = 1;
        }

        xQueueSend(queue_app_event, &evt, portMAX_DELAY);
    }
}
