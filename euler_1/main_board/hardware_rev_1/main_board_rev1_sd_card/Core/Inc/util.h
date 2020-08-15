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


typedef enum {
	A = 1,
	B,
	C,
	D
} rocket_state;

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
	IMU = 1,
	BARO,
	GPS
} sensor_t;

typedef union {
	imu_data imu;
	baro_data baro;
} sensor_u;

typedef enum {
	SENSOR = 1,
	STATE,
	ESTIMATOR_VAR,
	STR
} log_entry_t;

typedef struct {
	sensor_t sens_type;
	sensor_u sensor;
	uint8_t sensor_board_id;
} sensor_entry_t;


typedef union {
	sensor_entry_t sens_entry;
	rocket_state state_entry;
	float est_var_entry;
	char str_entry[56];
} entry_u;

typedef struct {
	log_entry_t entry_type;
	entry_u entry_u;
	uint32_t timestamp;
} log_row_t;

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
