#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "drivers/padSensor/padSens.h"
#include "app/app_events.h"

extern QueueHandle_t queue_app_event;
extern SemaphoreHandle_t sem_touchPad;
static const char *TAG = "pad_task";

static void pad_isr(void *arg)
{
    // ESP_LOGI(TAG, "pad_isr");
    xSemaphoreGiveFromISR(sem_touchPad, NULL);
}

void pad_task(void *arg)
{
    ESP_LOGI(TAG, "pad_task starts");

    padSens_init(pad_isr, NULL);

    while (1)
    {
        app_event_t evt;

        // wait pad has been pressed
        xSemaphoreTake(sem_touchPad, portMAX_DELAY);
        ESP_LOGI(TAG, "pad_task touch pad pressed!");

        evt.id = EVT_PAD_SENS;

        xQueueSend(queue_app_event, &evt, portMAX_DELAY);

        // Debounce software
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
