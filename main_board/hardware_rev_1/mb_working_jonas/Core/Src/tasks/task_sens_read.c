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
	HAL_SPI_Receive_IT(&hspi3, (uint8_t*) &sb3_data, sizeof(sb3_data));
//	uint8_t baro_buffer[12] = { 0 };
	for (;;) {
		tick_count += tick_update;

		if(osMutexAcquire(sb_3_mutexHandle, SB_MUTEX_TIMEOUT) == osOK) {
			sb3_baro.pressure = sb3_data.pressure;
			sb3_baro.temperature = sb3_data.temperature;
			sb3_baro.timestamp = tick_count;
			sb3_imu.acc_x = sb3_data.acc_x;
			sb3_imu.acc_y = sb3_data.acc_y;
			sb3_imu.acc_z = sb3_data.acc_z;
			sb3_imu.gyro_x = sb3_data.gyro_x;
			sb3_imu.gyro_y = sb3_data.gyro_y;
			sb3_imu.gyro_z = sb3_data.gyro_z;
			sb3_imu.timestamp = tick_count;
			osMutexRelease(sb_3_mutexHandle);
		}

		if(osMutexAcquire(sb_2_mutexHandle, SB_MUTEX_TIMEOUT) == osOK) {
			sb2_baro.pressure = sb3_data.pressure;
			sb2_baro.temperature = sb3_data.temperature;
			sb2_baro.timestamp = tick_count;
			sb2_imu.acc_x = sb3_data.acc_x;
			sb2_imu.acc_y = sb3_data.acc_y;
			sb2_imu.acc_z = sb3_data.acc_z;
			sb2_imu.gyro_x = sb3_data.gyro_x;
			sb2_imu.gyro_y = sb3_data.gyro_y;
			sb2_imu.gyro_z = sb3_data.gyro_z;
			sb2_imu.timestamp = tick_count;
			osMutexRelease(sb_2_mutexHandle);
		}

		if(osMutexAcquire(sb_1_mutexHandle, SB_MUTEX_TIMEOUT) == osOK) {
			sb1_baro.pressure = sb3_data.pressure;
			sb1_baro.temperature = sb3_data.temperature;
			sb1_baro.timestamp = tick_count;
			sb1_imu.acc_x = sb3_data.acc_x;
			sb1_imu.acc_y = sb3_data.acc_y;
			sb1_imu.acc_z = sb3_data.acc_z;
			sb1_imu.gyro_x = sb3_data.gyro_x;
			sb1_imu.gyro_y = sb3_data.gyro_y;
			sb1_imu.gyro_z = sb3_data.gyro_z;
			sb1_imu.timestamp = tick_count;
			osMutexRelease(sb_1_mutexHandle);
		}

		UsbPrint("[DBG] P: %ld; T: %ld; t: %lu\n", sb3_baro.pressure,
				sb3_baro.temperature, sb3_baro.timestamp);

		UsbPrint(
				"[DBG] Gx: %ld, Gy:%ld, Gz:%ld; Ax: %ld, Ay:%ld, Az:%ld; t: %lu\n",
				sb3_imu.gyro_x, sb3_imu.gyro_y,
				sb3_imu.gyro_z, sb3_imu.acc_x, sb3_imu.acc_y,
				sb3_imu.acc_z, sb3_imu.timestamp);




		osDelayUntil(tick_count);
	}
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
	HAL_SPI_Receive_IT(&hspi3, (uint8_t*) &sb3_data, sizeof(sb3_data));

}
