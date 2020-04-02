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
#include "util.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
/* Constants */
#define STATE_ESTIMATION_FREQUENCY 1
/* Matrix Sizes */
#define NUMBER_STATES 3	/* NUMBER_STATES x NUMBER_STATES -> A Matrix */
#define NUMBER_INPUTS 1	/* NUMBER_STATES x NUMBER_INPUTS -> B Matrix */
#define NUMBER_NOISE 1	/* NUMBER_STATES x NUMBER_NOISE -> G Matrix */
#define NUMBER_SENSOR 3 /* NUMBER_SENSOR x NUMBER_STATES -> H Matrix */
#define LAMBDA 0.1		/* Lambda for Moore Penrose Pseudoinverse */


/* Commands */

/* Extern */

/* Tasks */
void vTaskStateEst(void *argument);

#endif /* INC_TASKS_TASK_STATE_EST_H_ */
