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

sb_data_t sb1_data = { 0 };

/* Local Data Variable initialization */
imu_data_t last_imu_data = { 0 };
baro_data_t last_baro_data = { 0 };

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


		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi2, (uint8_t*) &fullsb_data, sizeof(fullsb_data), HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);




		osDelayUntil(tick_count);
	}
}


void ReadDataUSB(){

	memcpy(buffer, usb_data_buffer, 40);
	sb1_data.baro.pressure = (buffer[0]-48)*10000+(buffer[1]-48)*1000+(buffer[2]-48)*100+(buffer[3]-48)*10 + (buffer[4]-48);
	sb1_data.baro.temperature = (buffer[7]-48)*1000+(buffer[8]-48)*100+(buffer[9]-48)*10 + (buffer[10]-48);
	sb1_data.baro.ts = (buffer[12]-48)*10000+(buffer[13]-48)*1000+(buffer[14]-48)*100+(buffer[15]-48)*10 + (buffer[16]-48);


	if (buffer[28] == 45){
		sb1_data.imu.acc_z = -1*((buffer[29]-48)*1000+(buffer[30]-48)*100+(buffer[31]-48)*10 + (buffer[32]-48));
	}
	else {
		sb1_data.imu.acc_z = (buffer[29]-48)*1000+(buffer[30]-48)*100+(buffer[31]-48)*10 + (buffer[32]-48);
	}

	sb1_data.imu.ts = (buffer[34]-48)*10000+(buffer[35]-48)*1000+(buffer[36]-48)*100+(buffer[37]-48)*10 + (buffer[38]-48);
	fullsb_data = sb1_data;
	fullsb_data.checksum = fullsb_data.baro.pressure + fullsb_data.baro.temperature +
			fullsb_data.imu.gyro_x + fullsb_data.imu.gyro_y + fullsb_data.imu.gyro_z +
			fullsb_data.imu.acc_x + fullsb_data.imu.acc_y +  fullsb_data.imu.acc_z;

}

void ReadDataSensors(){
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
	fullsb_data.checksum = fullsb_data.baro.pressure + fullsb_data.baro.temperature +
			fullsb_data.imu.gyro_x + fullsb_data.imu.gyro_y + fullsb_data.imu.gyro_z +
			fullsb_data.imu.acc_x + fullsb_data.imu.acc_y +  fullsb_data.imu.acc_z;

	UsbPrint("[DBG] P: %ld; T: %ld; t: %lu; %lu\n", last_baro_data.pressure,
			last_baro_data.temperature, last_baro_data.ts, fullsb_data.checksum);

	UsbPrint(
			"[DBG Task Send] Gx: %ld, Gy:%ld, Gz:%ld; Ax: %ld, Ay:%ld, Az:%ld; t: %lu\n",
			last_imu_data.gyro_x, last_imu_data.gyro_y,
			last_imu_data.gyro_z, last_imu_data.acc_x, last_imu_data.acc_y,
			last_imu_data.acc_z, last_imu_data.ts);
}




