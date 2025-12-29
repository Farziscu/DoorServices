

#pragma once

#include "stdint.h"

typedef enum
{
    FP_NO_MATCH,
    FP_MATCH,
    FP_ERROR
} fp_result_t;

void fingerprint_init(void);
fp_result_t fingerprint_read(uint8_t *scr);
