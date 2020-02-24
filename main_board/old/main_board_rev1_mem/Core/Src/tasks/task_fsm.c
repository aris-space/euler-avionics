/*
 * task_fsm.c
 *
 *  Created on: Feb 1, 2020
 *      Author: stoja
 */

#include "typedef.h"
#include "tasks/task_fsm.h"

/* USER CODE BEGIN Header_vTaskFsm */
/**
 * @brief Function implementing the task_fsm thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_vTaskFsm */
void vTaskFsm(void *argument) {
	/* USER CODE BEGIN vTaskFsm */
	/* Infinite loop */
	for (;;) {
		osDelay(1);
	}
	/* USER CODE END vTaskFsm */
}
