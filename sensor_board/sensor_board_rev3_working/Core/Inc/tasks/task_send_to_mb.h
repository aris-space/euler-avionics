/*
 * task_send_to_mb.h
 *
 *  Created on: Nov 9, 2019
 *      Author: Jonas
 */

#ifndef INC_TASK_SEND_TO_MB_H_
#define INC_TASK_SEND_TO_MB_H_

/* Include */
#include "cmsis_os.h"
#include "typedef.h"
#include "main.h"

/* Tasks */
void vTaskSendToMb(void *argument);

/* Constants */
#define SPI_TIMEOUT 30
#define MB_SAMPLING_RATE 500

/* extern */
extern baro_data baro_data_to_mb;
extern imu_data imu_data_to_mb;
extern osMutexId_t baro_mutex;
extern osMutexId_t imu_mutex;
extern SPI_HandleTypeDef hspi2;

#endif /* INC_TASK_SEND_TO_MB_H_ */
