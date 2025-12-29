
#include "led.h"

static const gpio_num_t default_led_pin = GPIO_NUM_22;
static uint8_t led_state = 0;
static gpio_num_t led_pin = default_led_pin;

void led_initialize(gpio_num_t *ld)
{
    led_pin = (ld != NULL) ? *ld : default_led_pin;

    gpio_reset_pin(led_pin);

    gpio_set_direction(led_pin, GPIO_MODE_OUTPUT);

    gpio_hold_dis(led_pin);

    gpio_set_level(led_pin, led_state);
}

void led_toggle(void)
{
    led_state = !led_state;
    gpio_set_level(led_pin, led_state);
}

void led_switch_off(void)
{
    if (led_state)
    {
        led_state = !led_state;
        gpio_set_level(led_pin, led_state);
    }
}