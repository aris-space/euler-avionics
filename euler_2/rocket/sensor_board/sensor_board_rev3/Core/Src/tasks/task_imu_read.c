/*
 * task_imu_read.c
 *
 *  Created on: Nov 3, 2019
 *      Author: Jonas
 */

#include "tasks/task_imu_read.h"
#include "devices/icm20601.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"


void vInitImu20601();
void vReadImu20601(struct icm20601_dev * dev, int16_t gyroscope_data[], int16_t acceleration[], int16_t *temperature);

ICM20601 ICM1 = ICM20601_INIT1();
ICM20601 ICM2 = ICM20601_INIT2();

/**
 * @brief Function implementing the task_baro_read thread.
 * @param argument: Not used
 * @retval None
 */
void vTaskImuRead(void *argument) {
	uint32_t tick_count, tick_update;

	/* initialize data variables */
	int16_t gyroscope_data1[3] = { 0 }; /* 0 = x, 1 = y, 2 = z */
	int16_t acceleration1[3] = { 0 }; /* 0 = x, 1 = y, 2 = z */
	int16_t temperature1;

	int16_t gyroscope_data2[3] = { 0 }; /* 0 = x, 1 = y, 2 = z */
	int16_t acceleration2[3] = { 0 }; /* 0 = x, 1 = y, 2 = z */
	int16_t temperature2;

	/* initialize queue message */
	imu_data_t queue_data = { 0 };

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);


	vInitImu20601();

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / IMU20601_SAMPLING_FREQ;
	for (;;) {
		tick_count += tick_update;
		vReadImu20601(&ICM1, gyroscope_data1, acceleration1, &temperature1);
		vReadImu20601(&ICM2, gyroscope_data2, acceleration2, &temperature2);

		/* Debugging */

		UsbPrint("[DBG] RAW 1 Gx: %ld, Gy:%ld, Gz:%ld; Ax: %ld, Ay:%ld, Az:%ld, T:%ld; \n",
				gyroscope_data1[0], gyroscope_data1[1], gyroscope_data1[2],
				acceleration1[0], acceleration1[1], acceleration1[2], temperature1);
		UsbPrint("[DBG] RAW 2 Gx: %ld, Gy:%ld, Gz:%ld; Ax: %ld, Ay:%ld, Az:%ld, T:%ld; \n",
						gyroscope_data2[0], gyroscope_data2[1], gyroscope_data2[2],
						acceleration2[0], acceleration2[1], acceleration2[2], temperature2);

		//TODO HIE AUE STUFF WO MUES GMACHT WERDE MIT DENE DATE

		queue_data.gyro_x = gyroscope_data1[0];
		queue_data.gyro_y = gyroscope_data1[1];
		queue_data.gyro_z = gyroscope_data1[2];
		queue_data.acc_x = acceleration1[0];
		queue_data.acc_y = acceleration1[1];
		queue_data.acc_z = acceleration1[2];
		queue_data.ts = osKernelGetTickCount();

		/* Send Data to Queue */
		osMessageQueuePut(preprocess_queue, &queue_data, 0U, 0U);

		osDelayUntil(tick_count);
	}
}

void vInitImu20601() {
	osDelayUntil(1000);
	uint8_t r;
	do {
		r = icm20601_init(&ICM1);
		HAL_Delay(10);
	} while(!r);
	do {
		r = icm20601_init(&ICM2);
		HAL_Delay(10);
	} while(!r);
}

void vReadImu20601(struct icm20601_dev * dev, int16_t gyroscope_data[], int16_t acceleration[], int16_t *temperature) {
		icm20601_read_accel_raw(dev, acceleration);
		icm20601_read_gyro_raw(dev, gyroscope_data);
		icm20601_read_temp_raw(dev, temperature);
}

