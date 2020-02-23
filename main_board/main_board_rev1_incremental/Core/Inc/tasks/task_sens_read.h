/*
 * task_sens_read.h
 *
 *  Created on: Dec 26, 2019
 *      Author: stoja
 */

#ifndef INC_TASKS_TASK_SENS_READ_H_
#define INC_TASKS_TASK_SENS_READ_H_

#include "cmsis_os.h"
#include "typedef.h"
#include "main.h"

/* Constants */
#define SENSOR_READ_FREQUENCY 100
#define BARO_READ_TIMEOUT 30
/* Parameters */

/* Commands */

/* Extern */
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;

/* Tasks */
void vTaskSensRead(void *argument);

#endif /* INC_TASKS_TASK_SENS_READ_H_ */
