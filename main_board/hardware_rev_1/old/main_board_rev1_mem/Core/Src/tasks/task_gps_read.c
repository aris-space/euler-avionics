/*
 * task_gps_read.c
 *
 *  Created on: Jan 28, 2020
 *      Author: stoja
 */

#include "typedef.h"
#include "tasks/task_gps_read.h"

/* USER CODE BEGIN Header_vTaskGpsRead */
/**
 * @brief Function implementing the task_gps_read thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_vTaskGpsRead */
void vTaskGpsRead(void *argument) {
	gps_data rcv_gps1, rcv_gps2;
	for (;;) {
		/* acquire data from gps1 */

		/* acquire data from gps2 */

		if (osMutexAcquire(gps_mutex, GPS_MUTEX_TIMEOUT) == osOK) {
			gps1 = rcv_gps1;
			gps2 = rcv_gps2;
			osMutexRelease(gps_mutex);
		}
		osDelay(1);
	}

}
