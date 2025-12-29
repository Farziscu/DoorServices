
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_log.h"

#include "app_fsm.h"
#include "app_events.h"

extern QueueHandle_t queue_app_event;
// extern SemaphoreHandle_t sem_relay;
extern SemaphoreHandle_t sem_led;
extern TimerHandle_t sleep_timer;

static app_state_t state = APP_IDLE;
static const char *TAG = "app_fsm";
/*static const char *app_state_str[] = {
    "APP_IDLE",
    "APP_EXEC_CMD",
    "APP_ERROR"}; */

void app_fsm_init(void)
{
    state = APP_IDLE;
}

void app_send_event(app_event_t *evt)
{
    xQueueSend(queue_app_event, evt, portMAX_DELAY);
}

void app_fsm_handle_event(app_event_t *evt)
{
    // ESP_LOGI(TAG, "app_fsm_handle_event : %s", app_state_str[state]);

    // restart timer
    xTimerReset(sleep_timer, 0);

    switch (state)
    {
    case APP_IDLE:
        if (evt->id == EVT_FP_MATCH)
        {
            ESP_LOGI(TAG, "got EVT_FP_MATCH");
            app_event_t cmd = {
                .id = EVT_RELAY_PULSE,
                .data.relay_on = 0};
            app_send_event(&cmd);
            state = APP_EXEC_CMD;
        }
        else if (evt->id == EVT_TEMP_UPDATE)
        {
            ESP_LOGI(TAG, "got EVT_TEMP_UPDATE");

            // ESP_LOGI(TAG, "temp == %f", evt->data.temperature);
            // ESP_LOGI(TAG, "err  == %d", evt->data.error_code);

            ESP_LOGI(TAG, "temp == %.2f  press == %.2f ", evt->data.tempSens.temperature, evt->data.tempSens.pressure);
            /*
            app_event_t cmd = {
                .id = EVT_TEMP_UPDATE,
                .data.temperature = evt->data.temperature};

            app_send_event(&cmd);*/
            // state = APP_EXEC_CMD;
            state = APP_IDLE;
        }
        else if (evt->id == EVT_BTN_A)
        {
            ESP_LOGI(TAG, "got EVT_BTN_A");
            //...
            state = APP_EXEC_CMD;
        }
        else if (evt->id == EVT_PAD_SENS)
        {
            ESP_LOGI(TAG, "got EVT_PAD_SENS");
            xSemaphoreGive(sem_led);
            //...
            state = APP_IDLE;
        }
        break;

    case APP_EXEC_CMD:
        if (evt->id == EVT_RELAY_PULSE)
        {
            // xSemaphoreGive(sem_relay);
        }
        else if (evt->id == EVT_TEMP_UPDATE)
        {
            /*if (evt->data.temperature > 40.0)
            {
                app_event_t cmd = {
                    .id = EVT_ERROR, // EVT_RELAY_CMD,
                    .data.relay_on = 0};
                app_send_event(&cmd);
            }*/
        }
        state = APP_IDLE;
        break;
    case APP_ERROR:
        break;
    }
}
