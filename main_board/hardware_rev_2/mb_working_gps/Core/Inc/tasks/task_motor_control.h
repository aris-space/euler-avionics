/*
 * task_motor_control.h
 *
 *  Created on: May 23, 2020
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_MOTOR_CONTROL_H_
#define INC_TASKS_TASK_MOTOR_CONTROL_H_

/* Includes */
#include <Util/util.h>
#include "cmsis_os.h"
#include "main.h"
#include "EPOS4/epos4.h"


#define MOTOR_TASK_FREQUENCY 1

/* extern */
extern osMutexId_t controller_mutex;
extern int32_t controller_output_global;



void vTaskMotorCont(void *argument);

#endif /* INC_TASKS_TASK_MOTOR_CONTROL_H_ */
