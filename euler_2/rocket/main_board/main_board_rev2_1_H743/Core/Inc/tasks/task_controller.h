/*
 * task_controller.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_CONTROLLER_H_
#define INC_TASKS_TASK_CONTROLLER_H_

/* Includes */
#include "util/mutex.h"
#include "util/util.h"

/* Constants */

/* Are defined in controller_const.h */

/* Parameters */

/* Commands */

/* Extern */
extern custom_mutex_t controller_mutex;
extern custom_mutex_t state_est_mutex;
extern custom_mutex_t fsm_mutex;
extern state_est_data_t state_est_data_global;
extern int32_t controller_output_global;
extern flight_phase_detection_t global_flight_phase_detection;

/* Tasks */
void vTaskController(void *argument);

#endif /* INC_TASKS_TASK_CONTROLLER_H_ */
