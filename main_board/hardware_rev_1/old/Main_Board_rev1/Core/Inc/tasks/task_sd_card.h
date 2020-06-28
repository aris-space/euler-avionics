/*
 * task_sd_card.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_SD_CARD_H_
#define INC_TASKS_TASK_SD_CARD_H_

/* Includes */
#include "cmsis_os.h"

/* Constants */

/* Parameters */
#define SD_CARD_WRITE_FREQUENCY 1000
/* Commands */

/* Extern */

/* Tasks */
void vTaskSdCard(void *argument);

#endif /* INC_TASKS_TASK_SD_CARD_H_ */
