/*
 * task_buzzer.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */
#include "tasks/task_buzzer.h"

void vTaskBuzzer(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / BUZZER_FREQUENCY;
	for (;;) {
		tick_count += tick_update;
		/* TODO: Actual Stuff */

		osDelayUntil(tick_count);
	}
}
