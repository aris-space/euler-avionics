/*
 * task_controller.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_CONTROLLER_H_
#define INC_TASKS_TASK_CONTROLLER_H_

/* Includes */
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <Util/util.h>
#include "Sim_Con/controller.h"

/* Constants */

/* Are defined in controller_const.h */

/* Parameters */

/* Commands */

/* Extern */
extern osMutexId_t controller_mutex;
extern osMutexId_t state_est_mutex;
extern osMutexId_t fsm_mutex;
extern state_est_data_t state_est_data_global;
extern int32_t controller_output_global;
extern flight_phase_detection_t global_flight_phase_detection;

/* Tasks */
void vTaskController(void *argument);

#endif /* INC_TASKS_TASK_CONTROLLER_H_ */
