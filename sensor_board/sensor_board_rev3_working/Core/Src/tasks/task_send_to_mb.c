/*
 * task_send_to_mb.c
 *
 *  Created on: Nov 9, 2019
 *      Author: Jonas
 */

/* include */
#include "tasks/task_send_to_mb.h"

void vTaskSendToMb(void *argument) {

	/* Local Data Variable initialization */
	imu_data last_imu_data = { 0 };
	baro_data last_baro_data = { 0 };
//	uint8_t imu_buffer[28] = { 0 };
	uint8_t baro_buffer[12] = { 0 };
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
		/* Put data into the buffer */
		for (int i = 0; i < 4; i++) {
			baro_buffer[i] = last_baro_data.pressure >> (3 - i) * 8;
			baro_buffer[i + 4] = last_baro_data.temperature >> (3 - i) * 8;
			baro_buffer[i + 8] = last_baro_data.timestamp >> (3 - i) * 8;
		}
		/* print data to Console */

		UsbPrint("[DBG] P: %ld; T: %ld; t: %lu\n", last_baro_data.pressure,
				last_baro_data.temperature, last_baro_data.timestamp);
		UsbPrint(
				"[DBG] Gx: %ld, Gy:%ld, Gz:%ld; Ax: %ld, Ay:%ld, Az:%ld; t: %lu\n",
				last_imu_data.gyro_x, last_imu_data.gyro_y,
				last_imu_data.gyro_z, last_imu_data.acc_x, last_imu_data.acc_y,
				last_imu_data.acc_z, last_imu_data.timestamp);

		/* send data to MB */
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi2, baro_buffer, BARO_STRUCT_SIZE, SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		osDelayUntil(tick_count);
	}

}

