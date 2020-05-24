/*
 * task_fsm.h
 *
 *  Created on: May 24, 2020
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_FSM_H_
#define INC_TASKS_TASK_FSM_H_

/* Includes */
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include "util.h"

/* Constants */
#define FSM_SAMPLING_FREQ 1

/* Parameters */

/* Commands */

/* Extern */
/* State Estimation Mutex */
extern osMutexId_t state_est_mutex;
extern state_est_data_t state_est_data;

/* Tasks */
void vTaskFSM(void *argument);

#endif /* INC_TASKS_TASK_FSM_H_ */
