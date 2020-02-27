/*
 * util.h
 *
 *  Created on: Feb 24, 2020
 *      Author: stoja
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_

#include "cmsis_os.h"

#define LOG_QUEUE_SIZE 128

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

typedef enum {
	IMU,
	BARO
} sensor_t;


typedef union {
	imu_data imu;
	baro_data baro;
} sensor_u;

typedef struct {
	sensor_t sens_type;
	sensor_u sensor;
	uint32_t timestamp;
	uint8_t sensor_board_id;
} log_entry_t;

/* Debug flag */
#ifdef DEBUG
#undef DEBUG
#endif
/* Comment the next line in order to disable debug mode */
#define DEBUG

/* Functions */
uint8_t UsbPrint(const char *format, ...);

extern osMessageQueueId_t log_queue;

#ifdef DEBUG
osMutexId_t print_mutex;
#define PRINT_BUFFER_LEN 200
char print_buffer[PRINT_BUFFER_LEN];
#endif

#endif /* INC_UTIL_H_ */
