#include "freertos/FreeRTOS.h"
#include "esp_sleep.h"
#include "esp_log.h"

// #include "sleep_manager.h"

#define TOUCH_LIGHT_SLEEP_WAKEUP 1

static const char *TAG = "sleep_manager";

void sleep_manager_start(void)
{
#if TOUCH_LIGHT_SLEEP_WAKEUP
    // while (1)
    {
        printf("Entering light sleep in:\n");
        for (int i = 0; i < 5; i++)
        {
            printf("%d sec...\n", 5 - i);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        /* Enter the light sleep */
        esp_light_sleep_start();
        /* Keep executing the code after waking up from the light sleep */
        if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TOUCHPAD)
        {
            ESP_LOGI(TAG, "Wake up by touch\n");
        }
        else
        {
            ESP_LOGE(TAG, "Wake up by other source\n");
            abort();
        }
    }
#else
    printf("Entering deep sleep in:\n");
    for (int i = 0; i < 5; i++)
    {
        printf("%d sec...\n", 5 - i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    /* Wait the UART to finish printing the log */
    uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
    /* Enter the deep sleep */
    esp_deep_sleep_start();
#endif
}
