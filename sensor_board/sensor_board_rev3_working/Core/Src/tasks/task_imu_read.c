/*
 * task_imu_read.c
 *
 *  Created on: Nov 3, 2019
 *      Author: Jonas
 */

#include "tasks/task_imu_read.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

void vInitImu20600Read();
void vReadImu20600(int32_t gyroscope_data[], int32_t acceleration[]);

/**
 * @brief Function implementing the task_baro_read thread.
 * @param argument: Not used
 * @retval None
 */
void vTaskImuRead(void *argument) {
	uint32_t tick_count, tick_update;
//	/* initialize data variables */
	int32_t gyroscope_data[3] = { 5, 10, 15 }; /* 0 = x, 1 = y, 2 = z */
	int32_t acceleration[3] = { 50, 100, 200 }; /* 0 = x, 1 = y, 2 = z */

	/* initialize counter as we want to average over 4 samples every time */
	int8_t counter = 0;

	/* initialize queue message */
	imu_data queue_data = { 0 };

	vInitImu20600Read();

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / IMU20600_SAMPLING_FREQ;
	for (;;) {
		tick_count += tick_update;
		/* enable read of the IMU */
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
		vReadImu20600(gyroscope_data, acceleration);
		/* disable read of the IMU */
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
		/* Debugging */

//		UsbPrint("[DBG] RAW Gx: %ld, Gy:%ld, Gz:%ld; Ax: %ld, Ay:%ld, Az:%ld; t: %lu\n",
//				gyroscope_data[0], gyroscope_data[1], gyroscope_data[2],
//				acceleration[0], acceleration[1], acceleration[2], tick_count);

//TODO [nstojosk] : can this overflow?
		queue_data.gyro_x += gyroscope_data[0];
		queue_data.gyro_y += gyroscope_data[1];
		queue_data.gyro_z += gyroscope_data[2];
		queue_data.acc_x += acceleration[0];
		queue_data.acc_y += acceleration[1];
		queue_data.acc_z += acceleration[2];
		++counter;
		if (counter == 4) {
			/* reset counter */
			counter = 0;

			/* Average Data */
			queue_data.gyro_x /= 4;
			queue_data.gyro_y /= 4;
			queue_data.gyro_z /= 4;
			queue_data.acc_x /= 4;
			queue_data.acc_y /= 4;
			queue_data.acc_z /= 4;
			/* I dont know it this works with the overflow of the tick_count! */
			queue_data.timestamp = tick_count;

			/* Send Data to Queue */
			osMessageQueuePut(preprocess_queue, &queue_data, 0U, 0U);

			/* reset queue value */
			queue_data = EMPTY_IMU;
			counter = 0;
		}
		osDelayUntil(tick_count);
	}
}

void vInitImu20600Read() {
	osDelayUntil(2000);
	while (1) {
		HAL_StatusTypeDef Test = HAL_ERROR;
		/* Disable I2C Mode */
		uint8_t register_sensor_control[2] = { 0 };
		register_sensor_control[0] = (0 << 7 | IMU20600_COMMAND_USER_CONTROL);
		register_sensor_control[1] = (SENS_FIFO_EN << 6 | I2C_DISABLE << 4 |
				SENS_FIFO_RST << 2);
		while (Test != HAL_OK) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
			Test = HAL_SPI_Transmit(&hspi1, register_sensor_control,
			IMU20600_COMMAND_LENGTH, IMU20600_SPI_TIMEOUT);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		}
		Test = HAL_ERROR;




		uint8_t whoamI = 1 << 7 | 0x75;
		uint8_t whoamI2 = { 0 };
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
		Test = HAL_SPI_Transmit(&hspi1, &whoamI, 1, IMU20600_SPI_TIMEOUT);
		Test = HAL_SPI_Receive(&hspi1, &whoamI2, 1, IMU20600_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);





		/* Configure Gyroscope */
		/* we need to send two bytes: first bit: write 0 /read 1, 7 next bits register address, the next 8 bits are the message */
		uint8_t register_gyro_config[2] = { 0 };
		register_gyro_config[0] = (0 << 7
				| IMU20600_COMMAND_GYROSCOPE_CONFIGURATION);
		register_gyro_config[1] = (GYRO_SELFTEST << 5 | GYRO_RANGE << 3
				| GYRO_FILTER);
		while (Test != HAL_OK) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
			Test = HAL_SPI_Transmit(&hspi1, register_gyro_config,
			IMU20600_COMMAND_LENGTH, IMU20600_SPI_TIMEOUT);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
		}
		Test = HAL_ERROR;
		osDelay(1000);
		/* Configure Accelerometer */
		uint8_t register_acc_config[2] = { 0 };
		register_acc_config[0] = (0 << 7
				| IMU20600_COMMAND_ACCELEROMETER_CONFIGURATION1);
		register_acc_config[1] = (ACC_SELFTEST << 5 | ACC_RANGE << 3);
		uint8_t register_acc_config2[2] = { 0 };
		register_acc_config2[0] = (0 << 7
				| IMU20600_COMMAND_ACCELEROMETER_CONFIGURATION2);
		register_acc_config2[1] = (ACC_AVGFILTER << 3 | ACC_FILTER);
		while (Test != HAL_OK) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
			Test = HAL_SPI_Transmit(&hspi1, register_acc_config,
			IMU20600_COMMAND_LENGTH, IMU20600_SPI_TIMEOUT);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
		}
		Test = HAL_ERROR;
		while (Test != HAL_OK) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
			Test = HAL_SPI_Transmit(&hspi1, register_acc_config2,
			IMU20600_COMMAND_LENGTH, IMU20600_SPI_TIMEOUT);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
		}
		Test = HAL_ERROR;

		/* FIFO disable */
		uint8_t register_FIFO[2] = { 0 };
		register_FIFO[0] = (0 << 7 | IMU20600_COMMAND_FIFO_ENABLE);
		register_FIFO[1] = (GYRO_FIFO_EN << 4 | ACC_FIFO_EN << 3);
		while (Test != HAL_OK) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
			Test = HAL_SPI_Transmit(&hspi1, register_FIFO,
			IMU20600_COMMAND_LENGTH,
			IMU20600_SPI_TIMEOUT);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
		}
		Test = HAL_ERROR;

		/* sensor management 1 */
		uint8_t register_sensor_powerMgmt1[2] = { 0 };
		register_sensor_powerMgmt1[0] = (0 << 7
				| IMU20600_COMMAND_POWER_MANAGMENT1);
		register_sensor_powerMgmt1[1] =
				(SENS_sleep_EN << 6 | SENS_clk_src << 0);
		while (Test != HAL_OK) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
			Test = HAL_SPI_Transmit(&hspi1, register_sensor_powerMgmt1,
			IMU20600_COMMAND_LENGTH, IMU20600_SPI_TIMEOUT);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
		}
		Test = HAL_ERROR;

		/* sensor management 2 */
		uint8_t register_sensor_powerMgmt2[2] = { 0 };
		register_sensor_powerMgmt2[0] = (0 << 7
				| IMU20600_COMMAND_POWER_MANAGMENT2);
		register_sensor_powerMgmt2[1] = (SENS_acc_axis_EN << 3
				| SENS_gyri_axis_EN << 0);
		while (Test != HAL_OK) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
			Test = HAL_SPI_Transmit(&hspi1, register_sensor_powerMgmt2,
			IMU20600_COMMAND_LENGTH, IMU20600_SPI_TIMEOUT);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
		}
		Test = HAL_ERROR;

		/* Test if what we measure is possible */
		int32_t gyroscope_data[3]; /* 0 = x, 1 = y, 2 = z */
		int32_t acceleration[3]; /* 0 = x, 1 = y, 2 = z */
		vReadImu20600(gyroscope_data, acceleration);
				UsbPrint("[DBG] RAW Gx: %ld, Gy:%ld, Gz:%ld; Ax: %ld, Ay:%ld, Az:%ld\n",
						gyroscope_data[0], gyroscope_data[1], gyroscope_data[2],
						acceleration[0], acceleration[1], acceleration[2]);
		uint32_t abs_value = acceleration[0] * acceleration[0]
				+ acceleration[1] * acceleration[1]
				+ acceleration[2] * acceleration[2];
		if (((abs_value > 0.25 && abs_value < 2.25)
				&& (gyroscope_data[0] > -50 && gyroscope_data[0] < 50
						&& gyroscope_data[1] > -50 && gyroscope_data[1] < 50
						&& gyroscope_data[2] > -50 && gyroscope_data[2] < 50))) {
			/* initialization successful */
			break;
		}
	}
}

void vReadImu20600(int32_t gyroscope_data[], int32_t acceleration[]) {
	/* Read Accelerometer Data */

	/*Im not sure if this is the right way -> if the buffers are read in the right way
	 * or if the adress byte is overwritten by the data, have to check here*/

	uint8_t bufferAcc[6] = { 0 };
	uint8_t commandaccread = 1 << 7 | IMU20600_COMMAND_ACC_READ;
	HAL_StatusTypeDef Test = HAL_ERROR;
	bufferAcc[0] = 1 << 7 | IMU20600_COMMAND_ACC_READ;
	uint8_t whoamI = 1 << 7 | 0x75;
	uint8_t whoamI2 = { 0 };
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
	Test = HAL_SPI_Transmit(&hspi1, &whoamI, 1, IMU20600_SPI_TIMEOUT);
	Test = HAL_SPI_Receive(&hspi1, &whoamI2, 1, IMU20600_SPI_TIMEOUT);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
	Test = HAL_SPI_TransmitReceive(&hspi1, commandaccread, bufferAcc, 6, IMU20600_SPI_TIMEOUT);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);



	acceleration[0] = bufferAcc[0] << 8 | bufferAcc[1];
	acceleration[1] = bufferAcc[2] << 8 | bufferAcc[3];
	acceleration[2] = bufferAcc[4] << 8 | bufferAcc[4];

	/* Read Gyroscope Data */
	uint8_t bufferGyro[6] = { 0 };
	uint8_t commandgyroread = 1 << 7 | IMU20600_COMMAND_GYRO_READ;
	bufferGyro[0] = 1 << 7 | IMU20600_COMMAND_GYRO_READ;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
	HAL_SPI_Transmit(&hspi1, bufferGyro, 6, IMU20600_SPI_TIMEOUT);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
	gyroscope_data[0] = bufferGyro[1] << 8 | bufferGyro[2];
	gyroscope_data[1] = bufferGyro[3] << 8 | bufferGyro[4];
	gyroscope_data[2] = bufferGyro[5] << 8 | bufferGyro[6];

}

