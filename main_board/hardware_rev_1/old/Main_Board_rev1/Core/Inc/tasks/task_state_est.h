/*
 * task_state_est.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_STATE_EST_H_
#define INC_TASKS_TASK_STATE_EST_H_

/* Includes */
#include "cmsis_os.h"

/* Constants */
#define STATE_ESTIMATION_FREQUENCY 1000
/* Parameters */

/* Commands */

/* Extern */

/* Tasks */
void vTaskStateEst(void *argument);

#endif /* INC_TASKS_TASK_STATE_EST_H_ */
