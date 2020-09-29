/*
 * task_send_to_mb.c
 *
 *  Created on: Nov 9, 2019
 *      Author: Jonas
 */

/* include */
#include "tasks/task_send_to_mb.h"


void ReadDataSensors();
void ReadDataUSB();

uint8_t baro_buffer[12] = { 0 };
uint8_t buffer[64] = {[0 ... 63] = 48};

/* SPI Send */
sb_data_t fullsb_data;

void vTaskSendToMb(void *argument) {



	/* For periodic update */
	uint32_t tick_count, tick_update;
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / MB_SAMPLING_RATE;

	for (;;) {
		tick_count += tick_update;



		if(USB_DATA_ENABLE){
			ReadDataUSB();
		}
		else{
			ReadDataSensors();
		}

		UsbPrint("[DBG Task Send Baro] P: %ld; T: %ld; t: %lu; %lu\n", fullsb_data.baro.pressure,
				fullsb_data.baro.temperature, fullsb_data.baro.ts, fullsb_data.checksum);

		UsbPrint(
				"[DBG Task Send IMU 1] Gx: %ld, Gy:%ld, Gz:%ld; Ax: %ld, Ay:%ld, Az:%ld; t: %lu\n",
				fullsb_data.imu_1.gyro_x, fullsb_data.imu_1.gyro_y,
				fullsb_data.imu_1.gyro_z, fullsb_data.imu_1.acc_x, fullsb_data.imu_1.acc_y,
				fullsb_data.imu_1.acc_z, fullsb_data.imu_1.ts);
		UsbPrint(
				"[DBG Task Send IMU 2] Gx: %ld, Gy:%ld, Gz:%ld; Ax: %ld, Ay:%ld, Az:%ld; t: %lu\n",
				fullsb_data.imu_2.gyro_x, fullsb_data.imu_2.gyro_y,
				fullsb_data.imu_2.gyro_z, fullsb_data.imu_2.acc_x, fullsb_data.imu_2.acc_y,
				fullsb_data.imu_2.acc_z, fullsb_data.imu_2.ts);


		HAL_SPI_Transmit(&hspi2, (uint8_t*) &fullsb_data, sizeof(fullsb_data), HAL_MAX_DELAY);




		osDelayUntil(tick_count);
	}
}


void ReadDataUSB(){

	memcpy(buffer, usb_data_buffer, 40);
	fullsb_data.baro.pressure = (buffer[0]-48)*10000+(buffer[1]-48)*1000+(buffer[2]-48)*100+(buffer[3]-48)*10 + (buffer[4]-48);
	fullsb_data.baro.temperature = (buffer[7]-48)*1000+(buffer[8]-48)*100+(buffer[9]-48)*10 + (buffer[10]-48);
	fullsb_data.baro.ts = (buffer[12]-48)*10000+(buffer[13]-48)*1000+(buffer[14]-48)*100+(buffer[15]-48)*10 + (buffer[16]-48);


	if (buffer[28] == 45){
		fullsb_data.imu_1.acc_z = -1*((buffer[29]-48)*1000+(buffer[30]-48)*100+(buffer[31]-48)*10 + (buffer[32]-48));
	}
	else {
		fullsb_data.imu_1.acc_z = (buffer[29]-48)*1000+(buffer[30]-48)*100+(buffer[31]-48)*10 + (buffer[32]-48);
	}

	fullsb_data.imu_1.ts = (buffer[34]-48)*10000+(buffer[35]-48)*1000+(buffer[36]-48)*100+(buffer[37]-48)*10 + (buffer[38]-48);
	fullsb_data.checksum = fullsb_data.baro.pressure + fullsb_data.baro.temperature +
			fullsb_data.imu_1.gyro_x + fullsb_data.imu_1.gyro_y + fullsb_data.imu_1.gyro_z +
			fullsb_data.imu_1.acc_x + fullsb_data.imu_1.acc_y +  fullsb_data.imu_1.acc_z;

}

void ReadDataSensors(){
	/* acquire current Data */

	/* IMU 1 */
	if (osMutexAcquire(imu_mutex_1, IMU_MUTEX_TIMEOUT) == osOK) {
		fullsb_data.imu_1 = imu_data_1_to_mb;
		osMutexRelease(imu_mutex_1);
	}

	/* IMU 2 */
	if (osMutexAcquire(imu_mutex_2, IMU_MUTEX_TIMEOUT) == osOK) {
		fullsb_data.imu_2 = imu_data_2_to_mb;
		osMutexRelease(imu_mutex_2);
	}

	/* Baro */
	if (osMutexAcquire(baro_mutex, BARO_MUTEX_TIMEOUT) == osOK) {
		fullsb_data.baro = baro_data_to_mb;
		osMutexRelease(baro_mutex);
	}


	fullsb_data.checksum = fullsb_data.baro.pressure + fullsb_data.baro.temperature +
			fullsb_data.imu_1.gyro_x + fullsb_data.imu_1.gyro_y + fullsb_data.imu_1.gyro_z +
			fullsb_data.imu_1.acc_x + fullsb_data.imu_1.acc_y +  fullsb_data.imu_1.acc_z +
			fullsb_data.imu_2.gyro_x + fullsb_data.imu_2.gyro_y + fullsb_data.imu_2.gyro_z +
			fullsb_data.imu_2.acc_x + fullsb_data.imu_2.acc_y +  fullsb_data.imu_2.acc_z;
}




