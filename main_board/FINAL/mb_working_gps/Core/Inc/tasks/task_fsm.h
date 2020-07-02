/*
 * task_fsm.h
 *
 *  Created on: May 24, 2020
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_FSM_H_
#define INC_TASKS_TASK_FSM_H_

/* Includes */
#include "Sim_Con/env.h"
#include "Sim_Con/flight_phase_detection.h"
#include "Util/util.h"
#include "Util/mutex.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "main.h"

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
void vTaskFSM(void *argument);


#endif /* INC_TASKS_TASK_FSM_H_ */
