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
/* Matrix Sizes */
#define A_SIZE 3	/* A_SIZE x A_SIZE -> A Matrix */
#define B_SIZE 1	/* A_SIZE x B_SIZE -> B Matrix */
#define G_SIZE 1	/* A_SIZE x G_SIZE -> G Matrix */
#define NUMBER_SENSOR 3 /* NUMBER_SENSOR x A_SIZE -> H Matrix */


/* Commands */

/* Extern */

/* Tasks */
void vTaskStateEst(void *argument);

#endif /* INC_TASKS_TASK_STATE_EST_H_ */
