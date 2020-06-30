/*
 * task_motor_control.h
 *
 *  Created on: May 23, 2020
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_MOTOR_CONTROL_H_
#define INC_TASKS_TASK_MOTOR_CONTROL_H_

/* Includes */
#include "Util/util.h"
#include "Util/mutex.h"
#include "cmsis_os.h"
#include "main.h"
#include "EPOS4/epos4.h"


#define MOTOR_TASK_FREQUENCY 20

/* extern */
extern custom_mutex_t controller_mutex;
extern int32_t controller_output_global;

extern custom_mutex_t command_mutex;
extern command_e global_telemetry_command;

extern custom_mutex_t fsm_mutex;
extern flight_phase_detection_t global_flight_phase_detection;



void vTaskMotorCont(void *argument);

#endif /* INC_TASKS_TASK_MOTOR_CONTROL_H_ */
