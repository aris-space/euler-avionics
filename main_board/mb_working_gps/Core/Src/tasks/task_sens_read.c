 /*
 * task_sens_read.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "tasks/task_sens_read.h"

void ReadDataSB();
void ReadDataUSB();

sb_data_t sb1_data = { 0 };
sb_data_t sb2_data = { 0 };
sb_data_t sb3_data = { 0 };

int32_t pressure = 101325;

void vTaskSensRead(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / SENSOR_READ_FREQUENCY;
	uint8_t checksum = 0;

	HAL_SPI_Receive_IT(&hspi3, (uint8_t*) &sb3_data, sizeof(sb3_data));

	for (;;) {
		tick_count += tick_update;


		if(READ_USB){
			ReadDataUSB();
		}
		else{
			ReadDataSB();
		}


		logSensor(tick_count, 3, BARO, &sb3_baro);
		logSensor(tick_count, 3, IMU, &sb3_imu);

		pressure = pressure + 100;
		osDelayUntil(tick_count);
	}
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
	HAL_SPI_Receive_IT(&hspi3, (uint8_t*) &sb3_data, sizeof(sb3_data));

}

void ReadDataSB(){
	if(osMutexAcquire(sb3_mutex, SB_MUTEX_TIMEOUT) == osOK) {
		sb3_baro = sb3_data.baro;
		sb3_baro.pressure = pressure;
		sb3_baro.temperature = 25*100;

		sb3_imu = sb3_data.imu;

		osMutexRelease(sb3_mutex);
	}

	if(osMutexAcquire(sb2_mutex, SB_MUTEX_TIMEOUT) == osOK) {
		sb2_baro = sb3_data.baro;
		sb2_baro.pressure = pressure;
		sb2_baro.temperature = 25*100;

		sb2_imu = sb3_data.imu;

		osMutexRelease(sb2_mutex);
	}

	if(osMutexAcquire(sb1_mutex, SB_MUTEX_TIMEOUT) == osOK) {
		sb1_baro = sb3_data.baro;
		sb1_baro.pressure = pressure;
		sb1_baro.temperature = 25*100;

		sb1_imu = sb3_data.imu;

		osMutexRelease(sb1_mutex);
	}
}

void ReadDataUSB(){

}
