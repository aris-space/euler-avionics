/*
 * util.h
 *
 *  Created on: Feb 24, 2020
 *      Author: stoja
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_

#include "cmsis_os.h"

/** BASIC TYPES **/

/* Timestamp */
typedef uint32_t timestamp_t;

/* Board ID */
typedef uint8_t board_id_t;


/** ENUMS **/

/* Rocket state */
typedef enum {
	A = 1, B, C, D
} rocket_state_e;

/* Sensor type */
typedef enum {
	BARO = 1, IMU, GPS
} sensor_type_e;

/* Log entry */
typedef enum {
	SENSOR = 1, STATE, ESTIMATOR_VAR, MSG
} log_entry_type_e;


/** SENSOR DATA TYPES **/

/* IMU data */
typedef struct {
	int32_t gyro_x, gyro_y, gyro_z;
	int32_t acc_x, acc_y, acc_z;
	timestamp_t ts;
} imu_data_t;

/* Barometer data */
typedef struct {
	int32_t pressure;
	int32_t temperature;
	timestamp_t ts;
} baro_data_t;

typedef struct {
	baro_data_t baro;
	imu_data_t imu;
} sb_data_t;

/* GPS data */
typedef struct {
	/* add more */
	timestamp_t ts;
} gps_data_t;

/* Sensor Board Mutexes */
osMutexId_t sb1_mutex;
osMutexId_t sb2_mutex;
osMutexId_t sb3_mutex;

static const imu_data_t EMPTY_IMU = { 0 };

/** LOGGING **/

#define LOG_BUFFER_LEN 150

#define LOG_QUEUE_SIZE 128
extern osMessageQueueId_t log_queue;

typedef struct {
	char str[LOG_BUFFER_LEN];
} log_entry_t;

osStatus_t logSensor(timestamp_t ts, board_id_t sensor_board_id,
		sensor_type_e sens_type, void *sensor_data);
osStatus_t logRocketState(timestamp_t ts, rocket_state_e rocket_state);
/* TODO [nstojosk] - this signature & implementation should be adjusted */
osStatus_t logEstimatorVar(timestamp_t ts, int32_t estimator_var_data);
osStatus_t logMsg(timestamp_t ts, char *msg);


/** USB DEBUGGING **/

/* Debug flag */
#ifdef DEBUG
#undef DEBUG
#endif
/* Comment the next line in order to disable debug mode */
#define DEBUG

#ifdef DEBUG
osMutexId_t print_mutex;
#define PRINT_BUFFER_LEN 200
char print_buffer[PRINT_BUFFER_LEN];
#endif

uint8_t UsbPrint(const char *format, ...);

#endif /* INC_UTIL_H_ */
