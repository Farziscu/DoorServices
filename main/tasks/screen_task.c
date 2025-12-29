#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "drivers/ssd1306/ssd1306.h"

static const char *TAG = "screen_task";

void screen_task(void *arg)
{
    ESP_LOGI(TAG, "screen_task starts");

    init_ssd1306();

    vTaskDelay(pdMS_TO_TICKS(3000));

    ssd1306_print_str(18, 0, "============", false);
    ssd1306_print_str(18, 17, "SSD1306 OLED", false);
    ssd1306_print_str(18, 27, "Door service", false);
    ssd1306_print_str(18, 47, "============", false);

    ssd1306_display();

    while (1)
    {

        // xSemaphoreTake(sem_led, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(portMAX_DELAY));
    }

    ESP_LOGI(TAG, "screen_task end");
}
