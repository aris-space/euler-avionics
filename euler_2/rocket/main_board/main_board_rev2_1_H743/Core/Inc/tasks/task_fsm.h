/*
 * task_fsm.h
 *
 *  Created on: May 24, 2020
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_FSM_H_
#define INC_TASKS_TASK_FSM_H_

/* Includes */
#include "sim_con/env.h"
#include "util/mutex.h"
#include "util/util.h"

/* Constants */
#define FSM_SAMPLING_FREQ 100

/* Parameters */

/* Commands */

/* Extern */

/** MUTEXES **/
extern custom_mutex_t fsm_mutex;
extern custom_mutex_t env_mutex;
extern custom_mutex_t state_est_mutex;
extern custom_mutex_t command_mutex;

/** GLOBAL VARIABLES **/
extern env_t global_env;
extern flight_phase_detection_t global_flight_phase_detection;
extern state_est_data_t state_est_data_global;
extern command_e global_telemetry_command;

/* Tasks */
void vTaskFsm(void *argument);

#endif /* INC_TASKS_TASK_FSM_H_ */
