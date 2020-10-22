/*
 * task_state_est.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_STATE_EST_H_
#define INC_TASKS_TASK_STATE_EST_H_

/* Includes */
#include "util/mutex.h"
#include "util/util.h"
#include "../aris-euler-state-estimation/Inc/env.h"
/* Constants */
/* -> Are in state_est_settings */

/* Defines */
#define BARO_LOWER_BOUND 70000
#define BARO_UPPER_BOUND 120000

#define TEMP_LOWER_BOUND -10
#define TEMP_UPPER_BOUND 60

/* Extern */
/* Sensor Board 1 */
extern custom_mutex_t sb1_mutex;
extern sb_data_t sb1_global;

/* Sensor Board 2 */
extern custom_mutex_t sb2_mutex;
extern sb_data_t sb2_global;

/* Sensor Board 3 */
extern custom_mutex_t sb3_mutex;
extern sb_data_t sb3_global;

/* State Estimation Mutex */
extern custom_mutex_t state_est_mutex;
extern state_est_data_t state_est_data_global;

/* fsm Mutex */
extern custom_mutex_t fsm_mutex;
extern custom_mutex_t environment_mutex;
extern flight_phase_detection_t global_flight_phase_detection;
extern env_t global_env;

/* Command Mutex */
extern custom_mutex_t command_mutex;
extern command_e global_telemetry_command;

/* Airbrake Extension Mutex */
extern custom_mutex_t airbrake_ext_mutex;
extern int32_t global_airbrake_ext_meas;

/* Tasks */
void vTaskStateEst(void *argument);

#endif /* INC_TASKS_TASK_STATE_EST_H_ */
