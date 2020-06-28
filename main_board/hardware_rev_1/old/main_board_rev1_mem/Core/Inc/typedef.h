/*
 * typedef.h
 *
 *  Created on: Feb 1, 2020
 *      Author: stoja
 */

#ifndef INC_TYPEDEF_H_
#define INC_TYPEDEF_H_


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
	imu_data imu;
	baro_data baro;
} sb_data;

typedef struct {
	int32_t pos_x, pos_y, pos_z;
	uint32_t timestamp;
} gps_data;



#define SB_MUTEX_TIMEOUT 30
#define GPS_MUTEX_TIMEOUT 30

#endif /* INC_TYPEDEF_H_ */
