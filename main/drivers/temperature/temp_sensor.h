
#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    float temperature;
    float pressure;
} temp_t;

void temp_sensor_init(void);
bool temperature_read(temp_t *values);
uint8_t temp_sensor_read(void);