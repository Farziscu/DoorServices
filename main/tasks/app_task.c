
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "app/app_fsm.h"
#include "app/app_events.h"

extern QueueHandle_t queue_app_event;
static const char *TAG = "app_task";

void app_task(void *arg)
{
    ESP_LOGI(TAG, "app_task starts");

    app_event_t evt;

    app_fsm_init();

    while (1)
    {
        if (xQueueReceive(queue_app_event, &evt, portMAX_DELAY))
        {
            app_fsm_handle_event(&evt);
        }
    }
}
