
#include "stdint.h"
#include <drivers/temperature/temperature.h>

#pragma once

typedef enum
{
    EVT_FP_MATCH,
    EVT_FP_NO_MATCH,
    EVT_FP_FAIL,
    EVT_TEMP_UPDATE,
    EVT_RELAY_PULSE,
    EVT_BTN_A,
    EVT_BTN_B,
    EVT_PAD_SENS,
    EVT_ERROR
} app_event_id_t;

typedef struct
{
    app_event_id_t id;
    union
    {
        uint8_t fp_score;
        temp_t tempSens;
        uint8_t relay_on; // 0 = off, 1 = on
        int error_code;
    } data;
} app_event_t;
