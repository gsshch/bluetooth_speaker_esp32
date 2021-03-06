/*
 * event.c
 *
 *  Created on: 2018-03-04 20:07
 *      Author: Jack Chen <redchenjs@live.com>
 */

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define TAG "event"

EventGroupHandle_t task_event_group;

void event_init(void)
{
    task_event_group = xEventGroupCreate();
}
