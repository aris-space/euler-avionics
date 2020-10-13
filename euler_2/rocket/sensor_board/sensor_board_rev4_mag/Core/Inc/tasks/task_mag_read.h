/*
 * task_mag_read.h
 *
 *  Created on: Oct 13, 2020
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_MAG_READ_H_
#define INC_TASKS_TASK_MAG_READ_H_

/* Includes */
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include "util.h"

/* Constants */
#if (SAMPLING_FREQ > 200)
#define MAGNO_SAMPLING_FREQ 200
#else
#define MAGNO_SAMPLING_FREQ SAMPLING_FREQ
#endif

#define MAGNO_MUTEX_TIMEOUT 10

/* Extern */
extern mag_data_t magno_data_to_mb;
extern osMutexId_t magno_mutex;

/* Tasks */
void vTaskMagRead(void *argument);

#endif /* INC_TASKS_TASK_MAG_READ_H_ */
