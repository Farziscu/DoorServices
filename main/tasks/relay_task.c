#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "app/app_events.h"

// extern SemaphoreHandle_t sem_relay;

void relay_task(void *arg)
{

    while (1)
    {
        // wait on binary semaphore
        // if (xSemaphoreTake(sem_relay, portMAX_DELAY))
        {
            // pulse :
            // 1 - close relay
            // 2 - wait 1.5sec
            // 3 - open relay
        }
    }
}
