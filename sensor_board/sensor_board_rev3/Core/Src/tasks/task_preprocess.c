/*
 * task_preprocess.c
 *
 *  Created on: Nov 8, 2019
 *      Author: Jonas
 */

#include "tasks/task_preprocess.h"

void vTaskPreprocess(void *argument) {
	/* registers for the Filter */
	int32_t registers[4] = { 0 };

	int32_t filtered_acc_z;
	int32_t raw_acc_z;

	/* Queue data and Status Initialisation */
	imu_data queue_data;

	/* loop delay */
	/* For periodic update */
	uint32_t tick_count, tick_update;
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / 3000;


	/* Infinite loop */
	for (;;) {
		tick_count += tick_update;
		if (osMessageQueueGet(preprocess_queue, &queue_data, 0U, 30U) == osOK) {
			raw_acc_z = queue_data.acc_data[2];
			filtered_acc_z = A0
					* (raw_acc_z + B1 * registers[0] + B2 * registers[1]
							+ B3 * registers[2] + B4 * registers[3]) +
			A1 * registers[0] + A2 * registers[1] + A3 * registers[2]
					+ A4 * registers[3];
			registers[3] = registers[2];
			registers[2] = registers[1];
			registers[1] = registers[0];
			registers[0] = raw_acc_z + B1 * registers[0] + B2 * registers[1]
					+ B3 * registers[2] + B4 * registers[3];
			/* Write Result into Motherboard Variable if Mutex is available */
			if (osMutexAcquire(imu_mutex, IMU_MUTEX_TIMEOUT) == osOK) {
				imu_data_to_mb = queue_data;
				imu_data_to_mb.acc_data[2] = filtered_acc_z; /* Insert filtered data */
				osMutexRelease(imu_mutex);
			}
		} else {
			/* TODO: wait for half a sampling time or something like that */
			osDelayUntil(tick_count);
		}
	}
}
