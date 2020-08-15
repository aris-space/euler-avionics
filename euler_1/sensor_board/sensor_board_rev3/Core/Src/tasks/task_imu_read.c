/*
 * task_imu_read.c
 *
 *  Created on: Nov 3, 2019
 *      Author: Jonas
 */

#include "tasks/task_imu_read.h"

void vInitImu20600Read();
void vReadImu20600(int32_t *gyroscope_data, int32_t *acceleration);

/**
 * @brief Function implementing the task_baro_read thread.
 * @param argument: Not used
 * @retval None
 */
void vTaskImuRead(void *argument) {
	uint32_t tick_count, tick_update;
	/* initialise variables for Data */
	int32_t gyroscope_data[3]; /* 0 = x, 1 = y, 2 = z */
	int32_t acceleration[3]; /* 0 = x, 1 = y, 2 = z */

	/* initialise counter as we want to average over 4 samples every time */
	int8_t counter;
	counter = 0;

	/* Initialise queue message */
	imu_data queue_data = { 0 };

	/* The chip select line has to be held high for the slave that is not active! */
	HAL_GPIO_WritePin(GPIOA, GPIO_ICM_CS_Pin, 0);
	HAL_GPIO_WritePin(GPIOA, GPIO_BMI_CS_Pin, 1);

	vInitImu20600Read();

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / IMU20600_SAMPLING_FREQ;
	for (;;) {
		tick_count += tick_update;
		vReadImu20600(gyroscope_data, acceleration);

		//TODO [nstojosk] : can this overflow?
		queue_data.gyro_data[0] += gyroscope_data[0];
		queue_data.gyro_data[1] += gyroscope_data[1];
		queue_data.gyro_data[2] += gyroscope_data[2];
		queue_data.acc_data[0] += acceleration[0];
		queue_data.acc_data[1] += acceleration[1];
		queue_data.acc_data[2] += acceleration[2];
		++counter;
		if (counter == 4) {
			/* reset counter */
			counter = 0;

			/* Average Data */
			queue_data.gyro_data[0] /= 4;
			queue_data.gyro_data[1] /= 4;
			queue_data.gyro_data[2] /= 4;
			queue_data.acc_data[0] /= 4;
			queue_data.acc_data[1] /= 4;
			queue_data.acc_data[2] /= 4;
			/* I dont know it this works with the overflow of the tick_count! */
			queue_data.timestamp = tick_count;

			/* Send Data to Queue */
			osMessageQueuePut(preprocess_queue, &queue_data, 0U, 30U);

			/* reset queue value */
			queue_data = EMPTY_IMU;
		}
		osDelayUntil(tick_count);
	}
}

void vInitImu20600Read() {
	while (1) {
		HAL_StatusTypeDef Test = HAL_ERROR;
		/* Configure Gyroscope */
		/* we need to send two bytes: first bit: write 0 /read 1, 7 next bits register address, the next 8 bits are the message */
		uint8_t register_gyro_config = (0 << 15
				| IMU20600_COMMAND_GYROSCOPE_CONFIGURATION << 8
				| GYRO_SELFTEST << 5 | GYRO_RANGE << 3 | GYRO_FILTER);
		while (Test != HAL_OK) {
			Test = HAL_SPI_Transmit(&hspi1, &register_gyro_config,
			IMU20600_COMMAND_LENGTH, IMU20600_SPI_TIMEOUT);
		}
		Test = HAL_ERROR;

		/* Configure Accelerometer */
		uint8_t register_acc_config = (0 << 15
				| IMU20600_COMMAND_ACCELEROMETER_CONFIGURATION1 << 8
				| ACC_SELFTEST << 5 | ACC_RANGE << 3);
		uint8_t register_acc_config2 = (0 << 15
				| IMU20600_COMMAND_ACCELEROMETER_CONFIGURATION2 << 8
				| ACC_FIFOSIZE << 6 | ACC_AVGFILTER << 4 | ACC_FILTER);
		while (Test != HAL_OK) {
			HAL_SPI_Transmit(&hspi1, &register_acc_config,
			IMU20600_COMMAND_LENGTH, IMU20600_SPI_TIMEOUT);
		}
		Test = HAL_ERROR;
		while (Test != HAL_OK) {
			HAL_SPI_Transmit(&hspi1, &register_acc_config2,
			IMU20600_COMMAND_LENGTH, IMU20600_SPI_TIMEOUT);
		}
		Test = HAL_ERROR;

		/* FIFO disable */
		uint8_t register_FIFO = (0 << 15 | IMU20600_COMMAND_FIFO_ENABLE << 8
				| GYRO_FIFO_EN << 4 | ACC_FIFO_EN << 3);
		while (Test != HAL_OK) {
			HAL_SPI_Transmit(&hspi1, &register_FIFO, IMU20600_COMMAND_LENGTH,
			IMU20600_SPI_TIMEOUT);
		}
		Test = HAL_ERROR;

		/* sensor control - Heidi did this last year, don't really know why this is used */
		uint8_t register_sensor_control = (0 << 15
				| IMU20600_COMMAND_USER_CONTROL << 8 | SENS_FIFO_EN << 6
				| SENS_FIFO_RST << 2);
		while (Test != HAL_OK) {
			HAL_SPI_Transmit(&hspi1, &register_sensor_control,
			IMU20600_COMMAND_LENGTH, IMU20600_SPI_TIMEOUT);
		}
		Test = HAL_ERROR;

		/* sensor management 1 */
		uint8_t register_sensor_powerMgmt1 = (0 << 15
				| IMU20600_COMMAND_POWER_MANAGMENT1 << 8 | SENS_sleep_EN << 6
				| SENS_clk_src << 0);
		while (Test != HAL_OK) {
			HAL_SPI_Transmit(&hspi1, &register_sensor_powerMgmt1,
			IMU20600_COMMAND_LENGTH, IMU20600_SPI_TIMEOUT);
		}
		Test = HAL_ERROR;

		/* sensor management 2 */
		uint8_t register_sensor_powerMgmt2 = (0 << 15
				| IMU20600_COMMAND_POWER_MANAGMENT2 << 8 | SENS_acc_axis_EN << 3
				| SENS_gyri_axis_EN << 0);
		while (Test != HAL_OK) {
			HAL_SPI_Transmit(&hspi1, &register_sensor_powerMgmt2,
			IMU20600_COMMAND_LENGTH, IMU20600_SPI_TIMEOUT);
		}
		Test = HAL_ERROR;

		/* Test if what we measure is possible */
		int32_t gyroscope_data[3]; /* 0 = x, 1 = y, 2 = z */
		int32_t acceleration[3]; /* 0 = x, 1 = y, 2 = z */
		vReadImu20600(gyroscope_data, acceleration);
		uint32_t abs_value = acceleration[0] * acceleration[0]
				+ acceleration[1] * acceleration[1]
				+ acceleration[2] * acceleration[2];
		if (!((abs_value > 0.25 && abs_value < 2.25)
				&& (gyroscope_data[0] > -50 && gyroscope_data[0] < 50
						&& gyroscope_data[1] > -50 && gyroscope_data[1] < 50
						&& gyroscope_data[2] > -50 && gyroscope_data[2] < 50))) {
			/* initialization unsuccessful */
			break;
		}
	}
}

void vReadImu20600(int32_t *gyroscope_data, int32_t *acceleration) {
	/* Read Accelerometer Data */

	/*Im not sure if this is the right way -> if the buffers are read in the right way
	 * or if the adress byte is overwritten by the data, have to check here*/

	uint8_t bufferAcc[7];

	bufferAcc[0] = 1 << 7 | IMU20600_COMMAND_ACC_READ;

	HAL_SPI_Receive(&hspi1, bufferAcc, 7, IMU20600_SPI_TIMEOUT);

	acceleration[0] = bufferAcc[1] << 8 | bufferAcc[2];
	acceleration[1] = bufferAcc[3] << 8 | bufferAcc[4];
	acceleration[2] = bufferAcc[5] << 8 | bufferAcc[6];

	/* Read Gyroscope Data */
	uint8_t bufferGyro[7];

	bufferGyro[0] = 1 << 7 | IMU20600_COMMAND_GYRO_READ;

	HAL_SPI_Receive(&hspi1, bufferGyro, 7, IMU20600_SPI_TIMEOUT);

	gyroscope_data[0] = bufferGyro[1] << 8 | bufferGyro[2];
	gyroscope_data[1] = bufferGyro[3] << 8 | bufferGyro[4];
	gyroscope_data[2] = bufferGyro[5] << 8 | bufferGyro[6];

}

