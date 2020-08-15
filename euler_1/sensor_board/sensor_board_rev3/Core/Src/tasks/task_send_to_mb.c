/*
 * task_send_to_mb.c
 *
 *  Created on: Nov 9, 2019
 *      Author: Jonas
 */

/* include */
#include "tasks/task_send_to_mb.h"

void toArray(uint8_t* array, int number);

void vTaskSendToMb(void *argument) {

	/* Local Data Variable initialisation */
	imu_data current_imu_data;
	baro_data current_baro_data;

	/* For periodic update */
	uint32_t tick_count, tick_update;
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / MOTHERBOARD_SAMPLING_RATE;

	for (;;) {
		tick_count += tick_update;
		if (osMutexAcquire(imu_mutex, IMU_MUTEX_TIMEOUT) == osOK) {
			current_imu_data = imu_data_to_mb;
			osMutexRelease(imu_mutex);
		}
		if (osMutexAcquire(baro_mutex, BARO_MUTEX_TIMEOUT) == osOK) {
			current_baro_data = baro_data_to_mb;
			osMutexRelease(baro_mutex);
		}

		/* TODO: SEND THE DATA TO THE MAINBOARD */

		/* This is for debugging purposes. I Do not know if this works yet. It probably doesn't.
		 * Why would it not work?
		 * - UART is on the wrong pins
		 * - Need to go over some USB interface -  maybe UART is not the way to go
		 * - The _write function in main.c is not visible here (should be tough...)
		 * - UART is not Initialised correctly.
		 */
		uint8_t buffer [10];
		toArray(buffer, current_baro_data.pressure);
		CDC_Transmit_FS(buffer, sizeof(buffer));

		osDelayUntil(tick_count);
	}

}

void toArray(uint8_t* array, int number)
    {
		int i = 0;
        while (number > 0){
        	array[i] = number%10;
        	number/=10;
        }
    }
