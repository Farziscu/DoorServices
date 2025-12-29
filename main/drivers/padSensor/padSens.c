
#include "freertos/FreeRTOS.h"
#include "inttypes.h"
#include "stdbool.h"

#include "driver/touch_version_types.h"
#include "driver/touch_sens.h"
#include "esp_log.h"

/***********/
/* DEFINES */
/***********/

#define EXAMPLE_TOUCH_CHAN_INIT_SCAN_TIMES 3
#define EXAMPLE_TOUCH_SAMPLE_CFG_NUM 1

#define EXAMPLE_TOUCH_SAMPLE_CFG_DEFAULT() {TOUCH_SENSOR_V1_DEFAULT_SAMPLE_CONFIG(5.0, TOUCH_VOLT_LIM_L_0V5, TOUCH_VOLT_LIM_H_1V7)}
#define EXAMPLE_TOUCH_CHAN_CFG_DEFAULT() {              \
    .abs_active_thresh = {1000},                        \
    .charge_speed = TOUCH_CHARGE_SPEED_7,               \
    .init_charge_volt = TOUCH_INIT_CHARGE_VOLT_DEFAULT, \
    .group = TOUCH_CHAN_TRIG_GROUP_BOTH,                \
}

/* DEFINE - END */

/************/
/* VARIABLE */
/************/
// If you want to change the wake-up channels, please make sure the channel GPIOs won't conflict to the EXT wakeup GPIOs
static int s_channel_id = TOUCH_MIN_CHAN_ID;

// Active threshold to benchmark ratio. (i.e., touch will be activated when data >= benchmark * (1 + ratio))
static float s_thresh2bm_ratio = 0.02f; // 2%

static const char *TAG = "padSens";
/* VARIABLE - END*/

/*
static bool default_touch_on_active_callback(touch_sensor_handle_t sens_handle, const touch_active_event_data_t *event, void *user_ctx)
{
    ESP_EARLY_LOGI("touch_callback", "[CH %d] active", (int)event->chan_id);
    return false;
}

static bool default_touch_on_inactive_callback(touch_sensor_handle_t sens_handle, const touch_inactive_event_data_t *event, void *user_ctx)
{
    ESP_EARLY_LOGI("touch_callback", "[CH %d] inactive", (int)event->chan_id);
    return false;
}
*/

static void padSens_initial_scanning(touch_sensor_handle_t sens_handle, touch_channel_handle_t chan_handle)
{
    /* Enable the touch sensor to do the initial scanning, so that to initialize the channel data */
    ESP_ERROR_CHECK(touch_sensor_enable(sens_handle));

    /* Scan the enabled touch channels for several times, to make sure the initial channel data is stable */
    for (int i = 0; i < EXAMPLE_TOUCH_CHAN_INIT_SCAN_TIMES; i++)
    {
        ESP_ERROR_CHECK(touch_sensor_trigger_oneshot_scanning(sens_handle, 2000));
    }

    /* Disable the touch channel to rollback the state */
    ESP_ERROR_CHECK(touch_sensor_disable(sens_handle));

    /* (Optional) Read the initial channel benchmark and reconfig the channel active threshold accordingly */
    ////////printf("Initial benchmark and new threshold are:\n");

    /* Read the initial benchmark of the touch channel */
    uint32_t benchmark = 0;

    /* Read smooth data instead if the touch V1 hardware does not support benchmark */
    ESP_ERROR_CHECK(touch_channel_read_data(chan_handle, TOUCH_CHAN_DATA_TYPE_SMOOTH, &benchmark));

    /* Calculate the proper active thresholds regarding the initial benchmark */
    //////////////printf("Touch [CH %d]", s_channel_id);
    /* Generate the default channel configuration and then update the active threshold based on the real benchmark */
    touch_channel_config_t chan_cfg = EXAMPLE_TOUCH_CHAN_CFG_DEFAULT();

    {
        // Touch V1 (ESP32) uses absolute threshold.
        chan_cfg.abs_active_thresh[0] = (uint32_t)(benchmark * (1 - s_thresh2bm_ratio));
        ///////////printf(" %" PRIu32 ", %" PRIu32 "\t", benchmark, chan_cfg.abs_active_thresh[0]);
    }
    ///////printf("\n");
    /* Update the channel configuration */
    ESP_ERROR_CHECK(touch_sensor_reconfig_channel(chan_handle, &chan_cfg));
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

esp_err_t padSens_init(void *onActiveCB, void *onInActiveCB)
{
    /* Handles of touch sensor */
    touch_sensor_handle_t sens_handle = NULL;
    touch_channel_handle_t chan_handle;

    /* Step 1: Create a new touch sensor controller handle with default sample configuration */
    touch_sensor_sample_config_t sample_cfg[TOUCH_SAMPLE_CFG_NUM] = EXAMPLE_TOUCH_SAMPLE_CFG_DEFAULT();
    touch_sensor_config_t sens_cfg = TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(EXAMPLE_TOUCH_SAMPLE_CFG_NUM, sample_cfg);
    ESP_ERROR_CHECK(touch_sensor_new_controller(&sens_cfg, &sens_handle));

    /* Step 2: Create and enable the new touch channel handles with default configurations */
    touch_channel_config_t chan_cfg = EXAMPLE_TOUCH_CHAN_CFG_DEFAULT();

    /* Allocate new touch channel on the touch controller */
    {
        ESP_ERROR_CHECK(touch_sensor_new_channel(sens_handle, s_channel_id, &chan_cfg, &chan_handle));
        /* Display the touch channel corresponding GPIO number, you can also know from `touch_sensor_channel.h` */
        touch_chan_info_t chan_info = {};
        ESP_ERROR_CHECK(touch_sensor_get_channel_info(chan_handle, &chan_info));
        ///////////printf("Touch [CH %d] enabled on GPIO%d\n", s_channel_id, chan_info.chan_gpio);
    }
    printf("=================================\n");

    /* Step 3: Confiture the default filter for the touch sensor (Note: Touch V1 uses software filter) */
    touch_sensor_filter_config_t filter_cfg = TOUCH_SENSOR_DEFAULT_FILTER_CONFIG();
    ESP_ERROR_CHECK(touch_sensor_config_filter(sens_handle, &filter_cfg));

    /* Step 4: Do the initial scanning to initialize the touch channel data
     * Without this step, the channel data in the first read will be invalid
     */
    padSens_initial_scanning(sens_handle, chan_handle);

    /* Step 5: Register the touch sensor callbacks, here only take `active` and `inactive` event for example */
    touch_event_callbacks_t callbacks = {
        .on_active = onActiveCB,     // default_touch_on_active_callback,
        .on_inactive = onInActiveCB, // default_touch_on_inactive_callback,
    };
    ESP_ERROR_CHECK(touch_sensor_register_callbacks(sens_handle, &callbacks, NULL));

    touch_sleep_config_t slp_cfg = TOUCH_SENSOR_DEFAULT_LSLP_CONFIG();
    ESP_ERROR_CHECK(touch_sensor_config_sleep_wakeup(sens_handle, &slp_cfg));

    /* Step 6: Enable the touch sensor */
    ESP_ERROR_CHECK(touch_sensor_enable(sens_handle));

    /* Step 7: Start continuous scanning, you can also trigger oneshot scanning manually */
    ESP_ERROR_CHECK(touch_sensor_start_continuous_scanning(sens_handle));

    return ESP_OK;
}
