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
#include "util.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* Constants */
#define CONTROLLER_SAMPLING_FREQ 1
#define POLY_DEG 30
#define OPT_TRAJ_CONTROL_INPUT 0
/* Parameters */

/* Commands */

/* Extern */

/* Tasks */
void vTaskController(void *argument);

#endif /* INC_TASKS_TASK_CONTROLLER_H_ */
