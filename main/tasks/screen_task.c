#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "drivers/screen/ssd1306.h"

static const char *TAG = "screen_task";

void screen_task(void *arg)
{
    ESP_LOGI(TAG, "screen_task starts");

    // init_ssd1306();
    oled_init();

    vTaskDelay(pdMS_TO_TICKS(3000));

    ssd1306_print_str(18, 0, "============", false);
    ssd1306_print_str(18, 17, "SSD1306 OLED", false);
    ssd1306_print_str(18, 27, "Door service", false);
    ssd1306_print_str(18, 47, "============", false);

    ssd1306_display();

    int count = 0;

    while (1)
    {

        vTaskDelay(pdMS_TO_TICKS(2000));

        if (count == 0)
        {
            ssd1306_clean_space(0, 127, 0, 63);
            count++;
        }
        else if (count == 1)
        {
            ssd1306_print_str(18, 0, "Hello World!", false);
            count++;
        }
        else if (count == 2)
        {
            ssd1306_print_str(18, 17, "SSD1306 OLED", false);
            count++;
        }
        else if (count == 3)
        {
            ssd1306_print_str(28, 27, "with ESP32", false);
            count++;
        }
        else if (count == 4)
        {
            ssd1306_print_str(38, 37, "ESP-IDF", false);
            count++;
        }
        else if (count == 5)
        {
            ssd1306_print_str(28, 47, "Embedded C", false);
            count++;
        }

        ssd1306_display();

        // vTaskDelay(pdMS_TO_TICKS(portMAX_DELAY));
    }

    ESP_LOGI(TAG, "screen_task end");
}
