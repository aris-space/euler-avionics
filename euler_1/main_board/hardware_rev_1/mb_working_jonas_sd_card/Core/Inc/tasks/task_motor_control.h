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


#define MOTOR_TASK_FREQUENCY 0.5

/* extern */
extern osMutexId_t motor_controller_mutex;


void vTaskMotorCont(void *argument);

#endif /* INC_TASKS_TASK_MOTOR_CONTROL_H_ */
