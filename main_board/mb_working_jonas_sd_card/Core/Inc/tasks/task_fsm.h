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
#include "Sensor_Fusion_Helper/env.h"

/* Constants */
#define FSM_SAMPLING_FREQ 1

/* Parameters */

/* Commands */

/* Extern */


/* FSM Mutex */
extern osMutexId_t fsm_mutex;
extern osMutexId_t environment_mutex;

extern env global_env;
extern flight_phase_detection_t global_flight_phase_detection;

/* State Estimation Mutex */
extern osMutexId_t state_est_mutex;
extern state_est_data_t state_est_data;

/* Tasks */
void vTaskFSM(void *argument);

#endif /* INC_TASKS_TASK_FSM_H_ */
