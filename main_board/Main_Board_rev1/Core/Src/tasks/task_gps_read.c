/*
 * task_gps_read.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "tasks/task_gps_read.h"

void vTaskGpsRead(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / GPS_READ_FREQUENCY;
	for (;;) {
		tick_count += tick_update;
		/* TODO: Actual Stuff */

		osDelayUntil(tick_count);
	}
}
