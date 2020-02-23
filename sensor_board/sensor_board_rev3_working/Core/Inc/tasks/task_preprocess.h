/*
 * task_preprocess.h
 *
 *  Created on: Nov 8, 2019
 *      Author: Jonas
 */

#ifndef INC_TASK_PREPROCESS_H_
#define INC_TASK_PREPROCESS_H_

/* Includes */
#include "cmsis_os.h"
#include "stm32l4xx_hal.h"
#include "typedef.h"
#include "main.h"

/* Lowpass Filter Constants */
static const double B1 = -0.44444444444;
static const double B2 = -0.0740740740;
static const double B3 = -0.0054869;
static const double B4 = -0.000152415;
static const double A0 = 0.95259868;
static const double A1 = 3.810394756;
static const double A2 = 5.71559208;
static const double A3 = 3.810394756;
static const double A4 = 0.95259868;

/* Tasks */
void vTaskPreprocess(void *argument);

/* Extern */
extern osMessageQueueId_t preprocess_queue;
extern osMutexId_t imu_mutex;
extern imu_data imu_data_to_mb;

#endif /* INC_TASK_PREPROCESS_H_ */
