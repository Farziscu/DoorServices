
#pragma once
#include "app_events.h"

typedef enum
{
    APP_IDLE,
    APP_EXEC_CMD,
    APP_ERROR
} app_state_t;

void app_fsm_init(void);
void app_fsm_handle_event(app_event_t *evt);