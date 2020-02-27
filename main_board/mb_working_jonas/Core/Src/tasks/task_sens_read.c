/*
 * task_sens_read.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "tasks/task_sens_read.h"

void vTaskSensRead(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / SENSOR_READ_FREQUENCY;
	for (;;) {
		tick_count += tick_update;
		/* TODO: Actual Stuff */
		if(osMutexAcquire(sb_1_mutexHandle, SB1_MUTEX_TIMEOUT) == osOK) {
			HAL_SPI_Receive(&hspi1, (uint8_t*) &sb1_baro, sizeof(sb1_baro), SB1_SPI_TIMEOUT);
		}
		osDelayUntil(tick_count);
	}
}
