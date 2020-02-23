/*
 * task_sen_fusion.c
 *
 *  Created on: Feb 1, 2020
 *      Author: stoja
 */

#include "typedef.h"
#include "tasks/task_sen_fusion.h"

/* USER CODE BEGIN Header_vTaskSensorFusion */
/**
 * @brief Function implementing the task_sen_fusion thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_vTaskSensorFusion */
void vTaskSensorFusion(void *argument) {
	for (;;) {
		/* gather data */

		osDelay(1);
	}
}
