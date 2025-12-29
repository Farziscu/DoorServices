
#include <stdio.h>
// #include <math.h>

#include <freertos/FreeRTOS.h>
#include "bmp280/bmp280.h"
#include <esp_log.h>

#include "temperature.h"

#define CONFIG_EXAMPLE_I2C_MASTER_SDA GPIO_NUM_21
#define CONFIG_EXAMPLE_I2C_MASTER_SCL GPIO_NUM_22

static const char *TAG = "drv-temperature";
static bmp280_t dev;
// static float simTemperature = 15;

bool temperature_init(void)
{
    // esp_log_level_set(TAG, ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(i2cdev_init());

    bmp280_params_t params;
    bmp280_init_default_params(&params);
    memset(&dev, 0, sizeof(bmp280_t));

    ESP_ERROR_CHECK(bmp280_init_desc(&dev, BMP280_I2C_ADDRESS_0, 0, CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));

    ESP_ERROR_CHECK(bmp280_init(&dev, &params));

    // bool bme280p = dev.id == BME280_CHIP_ID;
    // printf("BMP280: found 0x%X  --> %s\n", dev.id, bme280p ? "BME280" : "BMP280");
    ESP_LOGV(TAG, "BMP280: found 0x%X  --> %s\n", dev.id, (dev.id == BME280_CHIP_ID) ? "BME280" : "BMP280");
    return true;
}

tmp_result_t temperature_read(temp_t *values)
{
    if (values == NULL)
        return TMP_ERROR;

    float pressure, temperature, humidity;

    bmp280_force_measurement(&dev);
    vTaskDelay(pdMS_TO_TICKS(20));

    if (bmp280_read_float(&dev, &temperature, &pressure, &humidity) != ESP_OK)
    {
        ESP_LOGE(TAG, "Temperature/pressure reading failed\n");
        return TMP_ERROR;
    }

    values->temperature = temperature;
    values->pressure = pressure;

    ESP_LOGV(TAG, "__Pressure: %.2f Pa, Temperature: %.2f C", pressure, temperature);
    if (dev.id == BME280_CHIP_ID)
        ESP_LOGV(TAG, "Humidity: %.2f\n", humidity);

#if 0


    // simulazione...
    simTemperature += 0.3;

    //*tmp_value = simTemperature;
    *tmp_value = xTaskGetTickCount();

    // if (((int)simTemperature % 5) == 0)
    //     return TMP_ERROR;
    // else
#endif
    return TMP_OK;
}
