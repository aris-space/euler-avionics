/*
 * task_motor_control.h
 *
 *  Created on: May 23, 2020
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_MOTOR_CONTROL_H_
#define INC_TASKS_TASK_MOTOR_CONTROL_H_

/* Includes */
#include "cmsis_os.h"
#include "main.h"
#include "util.h"
#include <string.h>


#define MOTOR_TASK_FREQUENCY 0.5




/* external */
extern UART_HandleTypeDef huart7;


void vTaskMotorCont(void *argument);

#endif /* INC_TASKS_TASK_MOTOR_CONTROL_H_ */
