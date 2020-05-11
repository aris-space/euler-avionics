/*
 * task_send_to_mb.c
 *
 *  Created on: Nov 9, 2019
 *      Author: Jonas
 */

/* include */
#include "tasks/task_send_to_mb.h"
uint8_t baro_buffer[12] = { 0 };

void vTaskSendToMb(void *argument) {

	/* Local Data Variable initialization */
	imu_data_t last_imu_data = { 0 };
	baro_data_t last_baro_data = { 0 };

	/* For periodic update */
	uint32_t tick_count, tick_update;
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / MB_SAMPLING_RATE;

	for (;;) {
		tick_count += tick_update;

		/* acquire current Data */
		if (osMutexAcquire(imu_mutex, IMU_MUTEX_TIMEOUT) == osOK) {
			last_imu_data = imu_data_to_mb;
			osMutexRelease(imu_mutex);
		}

		if (osMutexAcquire(baro_mutex, BARO_MUTEX_TIMEOUT) == osOK) {
			last_baro_data = baro_data_to_mb;
			osMutexRelease(baro_mutex);
		}

		fullsb_data.baro = last_baro_data;
		fullsb_data.imu = last_imu_data;

//		UsbPrint("[DBG] P: %ld; T: %ld; t: %lu\n", last_baro_data.pressure,
//				last_baro_data.temperature, last_baro_data.ts);
//
//		UsbPrint(
//				"[DBG Task Send] Gx: %ld, Gy:%ld, Gz:%ld; Ax: %ld, Ay:%ld, Az:%ld; t: %lu\n",
//				last_imu_data.gyro_x, last_imu_data.gyro_y,
//				last_imu_data.gyro_z, last_imu_data.acc_x, last_imu_data.acc_y,
//				last_imu_data.acc_z, last_imu_data.ts);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi2, (uint8_t*) &fullsb_data, sizeof(fullsb_data), SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);




		osDelayUntil(tick_count);
	}
}


