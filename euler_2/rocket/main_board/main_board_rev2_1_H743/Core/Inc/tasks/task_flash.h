/*
 * task_flash.h
 *
 *  Created on: Aug 20, 2020
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_FLASH_H_
#define INC_TASKS_TASK_FLASH_H_

/* includes */

#include "util/mutex.h"
#include "util/util.h"
#include "drivers/flash/flash_driver.h"
#include "main.h"

/* defines */
#define FLASH_SAMPLING_FREQ 10

/* extern */

/* functions */
void vTaskFlash(void *argument);

#endif /* INC_TASKS_TASK_FLASH_H_ */
