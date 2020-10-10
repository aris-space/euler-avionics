/*
 * task_sens_read.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_SENS_READ_H_
#define INC_TASKS_TASK_SENS_READ_H_

/* Includes */

#include "Util/mutex.h"
#include "Util/util.h"
#include "main.h"

/* Constants */
#define SENSOR_READ_FREQUENCY 100
/* Parameters */

/* Commands */

/* Extern */
/* Sensor Board 1 */
extern custom_mutex_t sb1_mutex;
extern SPI_HandleTypeDef hspi1;
extern sb_data_t sb1_global;

/* Sensor Board 2 */
extern custom_mutex_t sb2_mutex;
extern SPI_HandleTypeDef hspi2;
extern sb_data_t sb2_global;

/* Sensor Board 3 */
extern custom_mutex_t sb3_mutex;
extern SPI_HandleTypeDef hspi3;
extern sb_data_t sb3_global;

/* USB Sensor Read In */
extern custom_mutex_t usb_data_mutex;
extern char usb_data_buffer[256];

/* Tasks */
void vTaskSensRead(void *argument);

#endif /* INC_TASKS_TASK_SENS_READ_H_ */
