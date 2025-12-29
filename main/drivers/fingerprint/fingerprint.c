
#include "fingerprint.h"
#include "esp_system.h"

void fingerprint_init(void)
{
    // niente HW, solo init logico
}

fp_result_t fingerprint_read(uint8_t *scr)
{
    // simulazione: 1 match ogni ~5 letture
    uint32_t r = 0; // esp_random() % 5;

    if (r == 0)
    {
        *scr = 100;
        return FP_MATCH;
    }
    else
        return FP_NO_MATCH;
}
