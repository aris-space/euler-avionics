/*
 * typedef.h
 *
 *  Created on: Nov 9, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TYPEDEF_H_
#define INC_TASKS_TYPEDEF_H_

/* include */
#include "cmsis_os.h"
/* used for UART USB debugging */
#include "string.h"
#include "stdio.h"

/* Message Type def */
typedef struct
{
	int32_t gyro_data[3];
	int32_t acc_data[3];
	uint32_t timestamp;
} imu_data;

static const imu_data EMPTY_IMU = { 0 };

typedef struct
{
	int32_t pressure;
	int32_t temperature;
	uint32_t timestamp;
} baro_data;

/* Constants */
#define QUEUE_SIZE 32

/* Functions */
void vinit_queue(osMessageQueueId_t queue);

#endif /* INC_TASKS_TYPEDEF_H_ */
