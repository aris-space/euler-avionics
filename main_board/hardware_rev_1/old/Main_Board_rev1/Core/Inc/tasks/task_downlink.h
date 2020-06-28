/*
 * task_downlink.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_DOWNLINK_H_
#define INC_TASKS_TASK_DOWNLINK_H_

/* Includes */
#include "cmsis_os.h"

/* Constants */
#define DOWNLINK_FREQUENCY 1000
/* Parameters */

/* Commands */

/* Extern */

/* Tasks */
void vTaskDownlink(void *argument);

#endif /* INC_TASKS_TASK_DOWNLINK_H_ */
