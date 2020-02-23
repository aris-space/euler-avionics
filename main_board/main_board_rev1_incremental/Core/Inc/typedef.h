/*
 * typedef.h
 *
 *  Created on: Dec 26, 2019
 *      Author: stoja
 */

#ifndef INC_TASKS_TYPEDEF_H_
#define INC_TASKS_TYPEDEF_H_

/* include */
#include "cmsis_os.h"
/* used for UART USB debugging */
#include <string.h>
#include <stdio.h>

/* IMU data structure */
typedef struct {
	int32_t gyro_x, gyro_y, gyro_z;
	int32_t acc_x, acc_y, acc_z;
	uint32_t timestamp;
} imu_data;

/* Barometer data structure */
typedef struct {
	int32_t pressure;
	int32_t temperature;
	uint32_t timestamp;
} baro_data;

/* Constants */
#define QUEUE_SIZE 32

/* Functions */
void vinit_queue(osMessageQueueId_t queue);

#endif /* INC_TASKS_TYPEDEF_H_ */
