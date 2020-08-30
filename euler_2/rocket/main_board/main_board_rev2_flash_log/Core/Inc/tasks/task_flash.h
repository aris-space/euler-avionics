/*
 * task_flash.h
 *
 *  Created on: Aug 20, 2020
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_FLASH_H_
#define INC_TASKS_TASK_FLASH_H_

/* includes */

#include "main.h"
#include "Util/util.h"
#include "Util/mutex.h"
#include "Flash/w25qxx.h"

/* defines */
#define FLASH_SAMPLING_FREQ 500

/* extern */

/* functions */
void vTaskFlash(void *argument);

#endif /* INC_TASKS_TASK_FLASH_H_ */
