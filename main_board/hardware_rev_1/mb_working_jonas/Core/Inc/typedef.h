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

typedef struct {
	int32_t pressure;
	int32_t temperature;
	uint32_t timestamp_b;
	int32_t gyro_x, gyro_y, gyro_z;
	int32_t acc_x, acc_y, acc_z;
	uint32_t timestamp_i;
} sb_data;

/* Constants */
#define QUEUE_SIZE 32
#define BARO_MUTEX_TIMEOUT 0
#define IMU_MUTEX_TIMEOUT 0

static const imu_data EMPTY_IMU = { 0 };

#endif /* INC_TASKS_TYPEDEF_H_ */
