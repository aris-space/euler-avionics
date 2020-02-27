/*
 * task_sens_read.c
 *
 *  Created on: Feb 26, 2020
 *      Author: stoja
 */

#include "util.h"
#include "cmsis_os.h"

#include "tasks/task_sens_read.h"

void vTaskSensRead(void *argument) {
	imu_data imu1 = { .gyro_x = 1312, .gyro_y = 764, .gyro_z = -123, .acc_x =
			42738, .acc_y = 321, .acc_z = 777, .timestamp = 3 };
	baro_data baro1 = { .pressure = 123, .temperature = 13321, .timestamp = 3 };

	log_entry_t log1 = { .sens_type = IMU, .sensor = { .imu = imu1 },
			.sensor_board_id = 0 };
	log_entry_t log2 = { .sens_type = BARO, .sensor = { .baro = baro1 },
			.sensor_board_id = 2 };

	for (;;) {
		log1.timestamp = osKernelGetTickCount();
		log2.timestamp = osKernelGetTickCount();
		//UsbPrint("[SENSOR READ TASK] Log elements in queue %ld!\n", osMessageQueueGetCount(log_queue));
		osMessageQueuePut(log_queue, &log1, 0U, 0U);
		osMessageQueuePut(log_queue, &log2, 0U, 0U);

		osDelay(100);
	}
}
