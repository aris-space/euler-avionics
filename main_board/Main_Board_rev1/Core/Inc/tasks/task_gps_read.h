/*
 * task_gps_read.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_GPS_READ_H_
#define INC_TASKS_TASK_GPS_READ_H_

/* Includes */
#include "cmsis_os.h"

/* Constants */
#define GPS_READ_FREQUENCY 10
/* Parameters */

/* Commands */

/* Extern */

/* Tasks */
void vTaskGpsRead(void *argument);

#endif /* INC_TASKS_TASK_GPS_READ_H_ */
