#include "temp_sensor.h"
// #include "i2c_bus.h"
#include <driver/i2c_master.h>
#include "esp_log.h"

#include "bmp280/bmp280.h"

#define BMP280_I2C_ADDRESS 0x76 //!< I2C address when SDO pin is low
#define BMP280_REG_ID 0xD0

#define CONFIG_EXAMPLE_I2C_MASTER_SDA GPIO_NUM_21
#define CONFIG_EXAMPLE_I2C_MASTER_SCL GPIO_NUM_22

static i2c_master_dev_handle_t temp_dev;
static bmp280_t dev;
static const char *TAG = "temp_sensor";

void temp_sensor_init(void)
{
    /*
    i2c_master_bus_handle_t bus = i2c_bus_get();

    i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BMP280_I2C_ADDRESS,
        .scl_speed_hz = 400000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &cfg, &temp_dev));
    */

    ESP_ERROR_CHECK(i2cdev_init());

    bmp280_params_t params;
    bmp280_init_default_params(&params);
    memset(&dev, 0, sizeof(bmp280_t));

    ESP_ERROR_CHECK(bmp280_init_desc(&dev, BMP280_I2C_ADDRESS_0, 0, CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));

    ESP_ERROR_CHECK(bmp280_init(&dev, &params));

    ESP_LOGV(TAG, "BMP280: found 0x%X  --> %s\n", dev.id, (dev.id == BME280_CHIP_ID) ? "BME280" : "BMP280");

    ESP_LOGI(TAG, "temperature sensor initialized");
}

bool temperature_read(temp_t *values)
{
    if (values == NULL)
        return false;

    float pressure, temperature, humidity;

    bmp280_force_measurement(&dev);
    vTaskDelay(pdMS_TO_TICKS(20));

    if (bmp280_read_float(&dev, &temperature, &pressure, &humidity) != ESP_OK)
    {
        ESP_LOGE(TAG, "Temperature/pressure reading failed\n");
        return false;
    }

    values->temperature = temperature;
    values->pressure = pressure;

    ESP_LOGV(TAG, "__Pressure: %.2f Pa, Temperature: %.2f C", pressure, temperature);
    if (dev.id == BME280_CHIP_ID)
        ESP_LOGV(TAG, "Humidity: %.2f\n", humidity);

    return true;
}

uint8_t temp_sensor_read(void)
{
    uint8_t reg = BMP280_REG_ID;
    uint8_t data[1];

    i2c_master_transmit_receive(temp_dev, &reg, 1, data, 2, 1000);

    // int16_t raw = (data[0] << 8) | data[1];
    // return raw * 0.0625f;
    return data[0];
}
