
#pragma once

// #define BUTTON_GPIO GPIO_NUM_0 // scegli il GPIO che vuoi
#define BUTTON1_GPIO GPIO_NUM_0
#define BUTTON2_GPIO GPIO_NUM_4

void btn_init(void);
void btn_task(void *arg);
