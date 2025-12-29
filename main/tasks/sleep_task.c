#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_sleep.h"
#include "esp_log.h"

//////////#include "freertos/timers.h" /////////canc

#include "services/sleep_manager.h"
#include "app/timers.h"

extern EventGroupHandle_t sleep_event_group;
extern SemaphoreHandle_t sem_temperature;
////////extern TimerHandle_t sleep_timer; /// canc

#define SLEEP_OK_TEMP (1 << 0)
// #define SLEEP_OK_WIFI (1 << 1)
// #define SLEEP_OK_APP (1 << 2)
#define SLEEP_OK_ALL (SLEEP_OK_TEMP) //| SLEEP_OK_WIFI | SLEEP_OK_APP)

static const char *TAG = "sleep_task";

void sleep_task(void *arg)
{
    ESP_LOGI(TAG, "sleep_task starts");

    while (1)
    {
        // wait timer
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

        // check if each task is in idle
        EventBits_t bits = xEventGroupGetBits(sleep_event_group);

        if ((bits & SLEEP_OK_ALL) == SLEEP_OK_ALL)
        {
            ESP_LOGI(TAG, "going to sleep...");
            vTaskDelay(pdMS_TO_TICKS(20));

            /* Set the wakeup timer */
            timers_wakeup_set();

            /* Enter the light sleep */
            esp_light_sleep_start();

            esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
            ESP_LOGV(TAG, "wakeup cause == %d", cause);

            switch (cause)
            {
            case ESP_SLEEP_WAKEUP_TOUCHPAD:
                ESP_LOGI(TAG, "Wake up by touch");
                break;

            case ESP_SLEEP_WAKEUP_TIMER:
                ESP_LOGI(TAG, "Wake up by timer");
                ///////////xTimerReset(sleep_timer, 0); // canc
                // xSemaphoreGive(sem_temperature);
                break;

            default:
                break;
            }
        }
        else
        {
            ESP_LOGI(TAG, "Sleep postponed, tasks busy");
        }
    }
}
