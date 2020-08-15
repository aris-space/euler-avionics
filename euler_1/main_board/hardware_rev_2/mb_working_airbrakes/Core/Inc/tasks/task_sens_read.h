/*
 * task_sens_read.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_SENS_READ_H_
#define INC_TASKS_TASK_SENS_READ_H_

/* Includes */
#include "Util/util.h"
#include "cmsis_os.h"
#include "main.h"
#include "Util/mutex.h"

/* Constants */
#define SENSOR_READ_FREQUENCY 100
/* Parameters */

/* Commands */

/* Extern */
/* Sensor Board 1 */
extern custom_mutex_t sb1_mutex;
extern SPI_HandleTypeDef hspi1;
extern baro_data_t sb1_baro;
extern imu_data_t sb1_imu;
extern sb_data_t sb1_data;

/* Sensor Board 2 */
extern custom_mutex_t sb2_mutex;
extern SPI_HandleTypeDef hspi2;
extern baro_data_t sb2_baro;
extern imu_data_t sb2_imu;
extern sb_data_t sb2_data;

/* Sensor Board 3 */
extern custom_mutex_t sb3_mutex;
extern SPI_HandleTypeDef hspi3;
extern baro_data_t sb3_baro;
extern imu_data_t sb3_imu;
extern sb_data_t sb3_data;

/* USB Sensor Read In */
extern custom_mutex_t usb_data_mutex;
extern uint8_t usb_data_buffer[256];

/* Xbee Command */
extern custom_mutex_t command_mutex;
extern command_e global_telemetry_command;

/* Tasks */
void vTaskSensRead(void *argument);

#endif /* INC_TASKS_TASK_SENS_READ_H_ */
