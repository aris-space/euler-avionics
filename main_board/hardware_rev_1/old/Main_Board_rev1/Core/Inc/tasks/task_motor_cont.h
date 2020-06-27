/*
 * task_motor_cont.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_MOTOR_CONT_H_
#define INC_TASKS_TASK_MOTOR_CONT_H_

/* Includes */
#include "cmsis_os.h"

/* Constants */
#define MOTOR_CONTROLLER_FREQUENCY 500
/* Parameters */

/* Commands */

/* Extern */

/* Tasks */
void vTaskMotorCont(void *argument);

#endif /* INC_TASKS_TASK_MOTOR_CONT_H_ */
