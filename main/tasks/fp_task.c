#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "drivers/fingerprint/fingerprint.h"
#include "app/app_events.h"

extern QueueHandle_t queue_app_event;

void fp_task(void *arg)
{
    fingerprint_init();

    while (1)
    {
        uint8_t score = 0;
        fp_result_t res = fingerprint_read(&score);

        app_event_t evt;

        if (res == FP_MATCH)
        {
            evt.id = EVT_FP_MATCH;
            evt.data.fp_score = score;
        }
        else if (res == FP_NO_MATCH)
            evt.id = EVT_FP_NO_MATCH;
        else
            evt.id = EVT_FP_FAIL;

        xQueueSend(queue_app_event, &evt, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
