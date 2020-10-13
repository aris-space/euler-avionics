/*
 * task_mag_read.c
 *
 *  Created on: Oct 13, 2020
 *      Author: Jonas
 */

#include "tasks/task_mag_read.h"

/**
 * @brief Function implementing the mag_task_read thread.
 * @param argument: Not used
 * @retval None
 */
void vTaskMagRead(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;

	/* TODO INTIALIZE MAGNETOMETER */
	mag_data_t local_magno_data;

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / MAGNO_SAMPLING_FREQ;

	while (1) {
		tick_count += tick_update;

		/* TODO Read Magnetometer */


		/* If the Mutex is acquired we write the data into the right variable */
		if (osMutexAcquire(magno_mutex, MAGNO_MUTEX_TIMEOUT) == osOK) {
			magno_data_to_mb = local_magno_data;
			magno_data_to_mb.ts = osKernelGetTickCount();
			osMutexRelease(magno_mutex);
		}

		osDelayUntil(tick_count);
	}
}
