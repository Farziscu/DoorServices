#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#include "drivers/led/led.h"

#define BLINK_NO 15
#define PERIOD_ON_OFF_MS 150
#define SLEEP_OK_TEMP (1 << 0)

extern SemaphoreHandle_t sem_led;
extern EventGroupHandle_t sleep_event_group;
static const char *TAG = "led_task";

void led_task(void *arg)
{
    ESP_LOGI(TAG, "led_task starts");

    gpio_num_t ledNo = GPIO_NUM_33; // 22;
    led_initialize(&ledNo);

    while (1)
    {
        uint8_t count;

        xEventGroupSetBits(sleep_event_group, SLEEP_OK_TEMP);

        xSemaphoreTake(sem_led, portMAX_DELAY);

        xEventGroupClearBits(sleep_event_group, SLEEP_OK_TEMP);

        ESP_LOGI(TAG, "led_task got");

        count = BLINK_NO;

        while (count--)
        {
            led_toggle();
            vTaskDelay(pdMS_TO_TICKS(PERIOD_ON_OFF_MS));
        }

        led_switch_off();
    }
}
