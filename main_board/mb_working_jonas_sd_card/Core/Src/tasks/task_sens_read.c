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
	//HAL_SPI_Receive_IT(&hspi3, (uint8_t*) &sb3_data, sizeof(sb3_data));

	for (;;) {
		tick_count += tick_update;


		if (HAL_SPI_Receive_IT(&hspi3, (uint8_t*) &sb3_data, sizeof(sb3_data)) == HAL_OK){
			//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		}

		if(osMutexAcquire(sb3_mutex, SB_MUTEX_TIMEOUT) == osOK) {
			sb3_baro = sb3_data.baro;
			sb3_imu = sb3_data.imu;
//			sb3_baro.timestamp = tick_count;
//			sb3_imu.timestamp = tick_count;
			osMutexRelease(sb3_mutex);
		}

		if(osMutexAcquire(sb2_mutex, SB_MUTEX_TIMEOUT) == osOK) {
			sb2_baro = sb3_data.baro;
			sb2_imu = sb3_data.imu;
//			sb2_baro.timestamp = tick_count;
//			sb2_imu.timestamp = tick_count;
			osMutexRelease(sb2_mutex);
		}

		if(osMutexAcquire(sb1_mutex, SB_MUTEX_TIMEOUT) == osOK) {
			sb1_baro = sb3_data.baro;
			sb1_imu = sb3_data.imu;
//			sb1_baro.timestamp = tick_count;
//			sb1_imu.timestamp = tick_count;
			osMutexRelease(sb1_mutex);
		}

		UsbPrint("[DBG] P: %ld; T: %ld; t: %lu\n", sb3_baro.pressure,
				sb3_baro.temperature, tick_count);

		UsbPrint(
				"[DBG] Gx: %ld, Gy:%ld, Gz:%ld; Ax: %ld, Ay:%ld, Az:%ld; t: %lu\n",
				sb3_imu.gyro_x, sb3_imu.gyro_y,
				sb3_imu.gyro_z, sb3_imu.acc_x, sb3_imu.acc_y,
				sb3_imu.acc_z, tick_count);

		logSensor(tick_count, 3, BARO, &sb3_baro);
		logSensor(tick_count, 3, IMU, &sb3_imu);

		osDelayUntil(tick_count);
	}
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
	HAL_SPI_Receive_IT(&hspi3, (uint8_t*) &sb3_data, sizeof(sb3_data));

}
