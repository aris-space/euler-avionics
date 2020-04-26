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
void vReadImu20600(uint32_t gyroscope_data[], uint32_t acceleration[]);

/**
 * @brief Function implementing the task_baro_read thread.
 * @param argument: Not used
 * @retval None
 */
void vTaskImuRead(void *argument) {
	uint32_t tick_count, tick_update;

	/* initialize data variables */
	uint32_t gyroscope_data[3] = { 5, 10, 15 }; /* 0 = x, 1 = y, 2 = z */
	uint32_t acceleration[3] = { 50, 100, 200 }; /* 0 = x, 1 = y, 2 = z */

	/* initialize counter as we want to average over 4 samples every time */
	int8_t counter = 0;

	/* initialize queue message */
	imu_data_t queue_data = { 0 };

	vInitImu20600Read();

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / IMU20601_SAMPLING_FREQ;
	for (;;) {
		tick_count += tick_update;
		vReadImu20600(gyroscope_data, acceleration);

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
			queue_data.ts = tick_count;

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
		HAL_Delay(100);
		/* sensor management 1 */
		/* Reset Sensor */
		uint8_t register_sensor_powerMgmt1[2] = { 0 };
		register_sensor_powerMgmt1[0] = IMU20601_COMMAND_POWER_MANAGMENT1_WRITE;
		register_sensor_powerMgmt1[1] =
				(reset_conf << 7 | SENS_sleep_EN << 6 | SENS_clk_src << 0);

		/* Write Register */

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, register_sensor_powerMgmt1,
				IMU20601_COMMAND_LENGTH, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		/* Wake Up Chip */
		register_sensor_powerMgmt1[1] =
				(SENS_sleep_EN << 6 | SENS_clk_src << 0);

		/* Write Register */

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, register_sensor_powerMgmt1,
				IMU20601_COMMAND_LENGTH, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		/* Read Register */

		uint8_t tx = 0xEB;
		uint8_t rx = 0;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, &tx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_SPI_Receive(&hspi1, &rx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		/* Disable I2C Mode */
		uint8_t register_sensor_control[2] = { 0 };
		register_sensor_control[0] = IMU20601_COMMAND_USER_CONTROL_WRITE;
		register_sensor_control[1] = (SENS_FIFO_EN << 6 | I2C_DISABLE << 4 | 1);

		/* Write Register */

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, register_sensor_control,
				IMU20601_COMMAND_LENGTH, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		/* Read Register */

		tx = 0xEA;
		rx = 0;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, &tx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_SPI_Receive(&hspi1, &rx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		/* Configure Gyroscope */
		uint8_t register_gyro_config[2] = { 0 };
		register_gyro_config[0] = IMU20601_COMMAND_GYROSCOPE_CONFIGURATION_WRITE;
		register_gyro_config[1] = (GYRO_SELFTEST << 5 | GYRO_RANGE << 3
				| GYRO_FILTER);

		/* Write Register */

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, register_gyro_config,
				IMU20601_COMMAND_LENGTH, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		/* Read Register */

		tx = 0x9B;
		rx = 0;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, &tx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_SPI_Receive(&hspi1, &rx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		/* Configure Accelerometer */
		uint8_t register_acc_config[2] = { 0 };
		register_acc_config[0] = IMU20601_COMMAND_ACCELEROMETER_CONFIGURATION1_WRITE;
		register_acc_config[1] = (ACC_SELFTEST << 5 | ACC_RANGE << 3);
		uint8_t register_acc_config2[2] = { 0 };
		register_acc_config2[0] = IMU20601_COMMAND_ACCELEROMETER_CONFIGURATION2_WRITE;
		register_acc_config2[1] = (ACC_AVGFILTER << 3 | ACC_FILTER);

		/* Write Register */

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, register_acc_config,
				IMU20601_COMMAND_LENGTH, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		/* Write Register */

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, register_acc_config2,
				IMU20601_COMMAND_LENGTH, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		/* Read Register */

		tx = 0x9C;
		rx = 0;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, &tx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_SPI_Receive(&hspi1, &rx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		/* Read Register */

		tx = 0x9D;
		rx = 0;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, &tx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_SPI_Receive(&hspi1, &rx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		/* FIFO disable */
		uint8_t register_FIFO[2] = { 0 };
		register_FIFO[0] = IMU20601_COMMAND_FIFO_ENABLE_WRITE;
		register_FIFO[1] = (GYRO_FIFO_EN << 4 | ACC_FIFO_EN << 3);

		/* Write Register */

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, register_FIFO,
				IMU20601_COMMAND_LENGTH,
				IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		/* Read Register */

		tx = 0xA3;
		rx = 0;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, &tx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_SPI_Receive(&hspi1, &rx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);


		/* sensor management 2 */
		uint8_t register_sensor_powerMgmt2[2] = { 0 };
		register_sensor_powerMgmt2[0] = IMU20601_COMMAND_POWER_MANAGMENT2_WRITE;
		register_sensor_powerMgmt2[1] = (SENS_acc_axis_EN << 3
				| SENS_gyri_axis_EN << 0);

		/* Write Register */

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, register_sensor_powerMgmt2,
				IMU20601_COMMAND_LENGTH, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		/* Read Register */

		tx = 0xEC;
		rx = 0;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, &tx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_SPI_Receive(&hspi1, &rx,
				1, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		uint8_t commandaccread = IMU20601_COMMAND_GYRO_READ;
		uint8_t commandaccread2 = IMU20601_COMMAND_GYRO_READ + 1;
		volatile uint8_t read = 0;
		volatile uint8_t read2 = 0;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, &commandaccread, 1, IMU20601_SPI_TIMEOUT);
		HAL_SPI_Receive(&hspi1, &read, 1, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, &commandaccread2, 1, IMU20601_SPI_TIMEOUT);
		HAL_SPI_Receive(&hspi1, &read2, 1, IMU20601_SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(1);


		/* Test if what we measure is possible */
		uint32_t gyroscope_data[3]; /* 0 = x, 1 = y, 2 = z */
		uint32_t acceleration[3]; /* 0 = x, 1 = y, 2 = z */
		vReadImu20600(gyroscope_data, acceleration);
		UsbPrint("[DBG] RAW Gx: %ld, Gy:%ld, Gz:%ld; Ax: %ld, Ay:%ld, Az:%ld\n",
				gyroscope_data[0], gyroscope_data[1], gyroscope_data[2],
				acceleration[0], acceleration[1], acceleration[2]);
//		uint32_t abs_value = acceleration[0] * acceleration[0]
//															+ acceleration[1] * acceleration[1]
//																							 + acceleration[2] * acceleration[2];
//		if (((abs_value > 0.25 && abs_value < 2.25)
//				&& (gyroscope_data[0] > -50 && gyroscope_data[0] < 50
//						&& gyroscope_data[1] > -50 && gyroscope_data[1] < 50
//						&& gyroscope_data[2] > -50 && gyroscope_data[2] < 50))) {
			/* initialization successful */
			break;
//		}
	}
}

void vReadImu20600(uint32_t gyroscope_data[], uint32_t acceleration[]) {

	/* Read Accelerometer Data */
	uint8_t bufferAcc[6] = { 0 };
	uint8_t commandaccread = IMU20601_COMMAND_ACC_READ;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &commandaccread, 1, IMU20601_SPI_TIMEOUT);
	HAL_SPI_Receive(&hspi1, bufferAcc, 6, IMU20601_SPI_TIMEOUT);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	acceleration[0] = bufferAcc[0] << 8 | bufferAcc[1];
	acceleration[1] = bufferAcc[2] << 8 | bufferAcc[3];
	acceleration[2] = bufferAcc[4] << 8 | bufferAcc[5];

	/* Read Gyroscope Data */
	uint8_t bufferGyro[6] = { 0 };
	uint8_t gyro[3] = { 0 };
	uint8_t commandgyroread = IMU20601_COMMAND_GYRO_READ;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &commandgyroread, 1, IMU20601_SPI_TIMEOUT);
	HAL_SPI_Receive(&hspi1, bufferGyro, 6, IMU20601_SPI_TIMEOUT);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	gyroscope_data[0] = bufferGyro[0] << 8 | bufferGyro[1];
	gyroscope_data[1] = bufferGyro[2] << 8 | bufferGyro[3];
	gyroscope_data[2] = bufferGyro[4] << 8 | bufferGyro[5];

}

