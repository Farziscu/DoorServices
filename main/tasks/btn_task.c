#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/queue.h"
// #include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"

#include "btn_task.h"

#include "app/app_events.h"

extern QueueHandle_t queue_app_event;
// static SemaphoreHandle_t button_sem;

EventGroupHandle_t button_event_group;
#define BUTTON1_BIT BIT0
#define BUTTON2_BIT BIT1

static void IRAM_ATTR button_isr_handler(void *arg)
{
    // BaseType_t hp_task_woken = pdFALSE;
    // xSemaphoreGiveFromISR(button_sem, &hp_task_woken);

    uint32_t gpio_num = (uint32_t)arg;
    BaseType_t hp_task_woken = pdFALSE;

    if (gpio_num == BUTTON1_GPIO)
        xEventGroupSetBitsFromISR(button_event_group, BUTTON1_BIT, &hp_task_woken);
    else if (gpio_num == BUTTON2_GPIO)
        xEventGroupSetBitsFromISR(button_event_group, BUTTON2_BIT, &hp_task_woken);

    if (hp_task_woken)
        portYIELD_FROM_ISR();
}

void btn_init(void)
{
    /*
    button_sem = xSemaphoreCreateBinary();

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE // HIGH → LOW
    };
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL);*/

    button_event_group = xEventGroupCreate();

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON1_GPIO) | (1ULL << BUTTON2_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE};
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON1_GPIO, button_isr_handler, (void *)BUTTON1_GPIO);
    gpio_isr_handler_add(BUTTON2_GPIO, button_isr_handler, (void *)BUTTON2_GPIO);
}

void btn_task(void *arg)
{
    EventBits_t bits;

    btn_init();

    while (1)
    {
        app_event_t evt;

        // wait btn A or btn B have been pressed
        /////////xSemaphoreTake(button_sem, portMAX_DELAY);

        bits = xEventGroupWaitBits(
            button_event_group,
            BUTTON1_BIT | BUTTON2_BIT,
            pdTRUE,  // reset dei bit dopo il read
            pdFALSE, // attendi anche più bit contemporaneamente
            portMAX_DELAY);

        evt.id = EVT_ERROR;

        if (bits & BUTTON1_BIT)
        {
            printf("Button 1 premuto!\n");
            // Azioni relative al button 1
            evt.id = EVT_BTN_A;
            xQueueSend(queue_app_event, &evt, portMAX_DELAY);
        }

        if (bits & BUTTON2_BIT)
        {
            printf("Button 2 premuto!\n");
            // Azioni relative al button 2
            evt.id = EVT_BTN_B;
            xQueueSend(queue_app_event, &evt, portMAX_DELAY);
        }

        // Debounce software
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
