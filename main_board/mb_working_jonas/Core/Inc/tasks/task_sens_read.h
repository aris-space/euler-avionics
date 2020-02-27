/*
 * task_sens_read.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_SENS_READ_H_
#define INC_TASKS_TASK_SENS_READ_H_

/* Includes */
#include "cmsis_os.h"
#include "main.h"
#include "typedef.h"

/* Constants */
#define SENSOR_READ_FREQUENCY 1000
#define SB1_MUTEX_TIMEOUT 30
#define SB1_SPI_TIMEOUT 30
/* Parameters */

/* Commands */

/* Extern */
/* Sensor Board 1 */
extern osMutexId_t sb_1_mutexHandle;
extern SPI_HandleTypeDef hspi1;
extern baro_data sb1_baro;
extern imu_data sb1_imu;

/* Tasks */
void vTaskSensRead(void *argument);

#endif /* INC_TASKS_TASK_SENS_READ_H_ */
