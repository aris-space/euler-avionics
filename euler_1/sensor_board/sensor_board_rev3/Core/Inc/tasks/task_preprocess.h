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
#define B1 -0.44444444444
#define B2 -0.0740740740
#define B3 -0.0054869
#define B4 -0.000152415
#define A0 0.95259868
#define A1 3.810394756
#define A2 5.71559208
#define A3 3.810394756
#define A4 0.95259868

/* Constants */
#define IMU_MUTEX_TIMEOUT 30

/* Tasks */
void vTaskPreprocess(void *argument);

/* Extern */
extern osMessageQueueId_t preprocess_queue;
extern osMutexId_t imu_mutex;
extern imu_data imu_data_to_mb;

#endif /* INC_TASK_PREPROCESS_H_ */
