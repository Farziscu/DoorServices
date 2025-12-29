
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#include "app/app_events.h"
#include "app/timers.h"
#include "tasks/app_task.h"
#include "tasks/fp_task.h"
#include "tasks/temp_task.h"
#include "tasks/relay_task.h"
#include "tasks/btn_task.h"
#include "tasks/pad_task.h"
#include "tasks/led_task.h"
#include "tasks/sleep_task.h"
#include "tasks/screen_task.h"
#include "services/sleep_manager.h"

#include "esp_sleep.h"

QueueHandle_t queue_app_event;
// SemaphoreHandle_t sem_relay;
SemaphoreHandle_t sem_led;
SemaphoreHandle_t sem_touchPad;
SemaphoreHandle_t sem_temperature;

EventGroupHandle_t sleep_event_group;

TaskHandle_t sleep_task_handle = NULL;
static const char *TAG = "DoorServices_main";

void createSemaphores(void);
void createQueues(void);

void app_main(void)
{
    ESP_LOGI(TAG, "app_main STARTS----------------------------");

    createSemaphores();
    createQueues();
    timers_init();

    sleep_event_group = xEventGroupCreate();
    configASSERT(sleep_event_group);

    if (xTaskCreate(app_task, "app_task", 4096, NULL, 6, NULL) != pdPASS)
    {
        printf("err!\n");
        ESP_LOGE(TAG, "Failed to create app_task");
    }

    xTaskCreate(pad_task, "pad_task", 2024, NULL, 4, NULL);

    xTaskCreate(led_task, "led_task", 2024, NULL, 3, NULL);

    xTaskCreate(temp_task, "temp_task", 4092, NULL, 2, NULL);

    xTaskCreate(sleep_task, "sleep_task", 2048, NULL, 2, &sleep_task_handle);

    // xTaskCreate(screen_task, "screen_task", 2048, NULL, 2, NULL);

    timers_wakeup_set();

    // got temperature soon
    xSemaphoreGive(sem_temperature);

    // xTaskCreate(fp_task, "fp_task", 2048, NULL, 5, NULL);
    // xTaskCreate(relay_task, "relay_task", 2048, NULL, 3, NULL);
    // xTaskCreate(btn_task, "btn_task", 2048, NULL, 3, NULL);
}

void createSemaphores(void)
{
    // sem_relay = xSemaphoreCreateBinary();
    sem_led = xSemaphoreCreateBinary();
    sem_touchPad = xSemaphoreCreateBinary();
    sem_temperature = xSemaphoreCreateBinary();
}

void createQueues(void)
{
    queue_app_event = xQueueCreate(5, sizeof(app_event_t));
}
