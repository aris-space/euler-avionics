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
#include "usbd_cdc_if.h"
#include "usb_device.h"
/* used for USB printing */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* Debug flag */
#ifdef DEBUG
#undef DEBUG
#endif
/* Comment the next line in order to disable debug mode */
#define DEBUG

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
#define BARO_STRUCT_SIZE 12

/* Constants */
#define QUEUE_SIZE 32
#define BARO_MUTEX_TIMEOUT 0
#define IMU_MUTEX_TIMEOUT 0

static const imu_data EMPTY_IMU = { 0 };

/* Functions */
uint8_t UsbPrint(const char *format, ...);


#ifdef DEBUG
osMutexId_t print_mutex;
#define PRINT_BUFFER_LEN 200
char print_buffer[PRINT_BUFFER_LEN];
#endif

#endif /* INC_TASKS_TYPEDEF_H_ */
