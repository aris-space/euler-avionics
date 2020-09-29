/*
 * task_gps.h
 *
 *  Created on: Jun 26, 2020
 *      Author: Luca
 */

#ifndef INC_TASKS_TASK_GPS_H_
#define INC_TASKS_TASK_GPS_H_

#include "Util/util.h"
#include "Util/mutex.h"
#include "stm32h7xx_hal.h"

#define GPS_SAMPLE_RATE 2

void vTaskGps(void *argument);

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern gps_data_t globalGPS;

extern custom_mutex_t gps_mutex;

#endif /* INC_TASKS_TASK_GPS_H_ */
