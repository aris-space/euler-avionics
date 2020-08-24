/*
 * task_flash.c
 *
 *  Created on: Aug 20, 2020
 *      Author: Jonas
 */
#include "tasks/task_flash.h"

void vTaskFlash(void *argument) {
	/* For periodic update */
	uint32_t tick_count, tick_update;

	osDelay(500);

	tick_update = osKernelGetTickFreq() / FLASH_SAMPLING_FREQ;
	tick_count = osKernelGetTickCount();

	while (1) {
		/* Tick Update */
		tick_count += tick_update;

		int32_t chip_id = 0;

		/* WORK */
		chip_id =  read_chip_id();

		UsbPrint("[FLASH]: ID: %ld\n", chip_id);


		/* Sleep */
		osDelayUntil(tick_count);
	}
}


