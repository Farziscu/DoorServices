
#pragma once

#include "driver/gpio.h"

void led_initialize(gpio_num_t *ld);
void led_toggle(void);
void led_switch_off(void);