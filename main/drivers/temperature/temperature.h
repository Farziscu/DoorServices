#pragma once

typedef struct
{
    float temperature;
    float pressure;
} temp_t;

typedef enum
{
    TMP_OK,
    TMP_ERROR
} tmp_result_t;

bool temperature_init(void);
tmp_result_t temperature_read(temp_t *values);