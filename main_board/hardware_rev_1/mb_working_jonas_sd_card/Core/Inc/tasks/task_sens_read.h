/*
 * task_sens_read.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_SENS_READ_H_
#define INC_TASKS_TASK_SENS_READ_H_

/* Includes */
#include <Util/util.h>
#include "cmsis_os.h"
#include "main.h"

/* Constants */
#define SENSOR_READ_FREQUENCY 1000
#define SB_MUTEX_TIMEOUT 30
#define SB1_SPI_TIMEOUT 30
#define SB3_SPI_TIMEOUT 30
/* Parameters */

/* Commands */

/* Extern */
/* Sensor Board 1 */
extern osMutexId_t sb_1_mutexHandle;
extern SPI_HandleTypeDef hspi1;
extern baro_data_t sb1_baro;
extern imu_data_t sb1_imu;
extern sb_data_t sb1_data;

/* Sensor Board 2 */
extern osMutexId_t sb_2_mutexHandle;
extern SPI_HandleTypeDef hspi2;
extern baro_data_t sb2_baro;
extern imu_data_t sb2_imu;
extern sb_data_t sb2_data;

/* Sensor Board 3 */
extern osMutexId_t sb_3_mutexHandle;
extern SPI_HandleTypeDef hspi3;
extern baro_data_t sb3_baro;
extern imu_data_t sb3_imu;
extern sb_data_t sb3_data;

/* Tasks */
void vTaskSensRead(void *argument);

#endif /* INC_TASKS_TASK_SENS_READ_H_ */
