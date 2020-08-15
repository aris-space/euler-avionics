/*
 * task_sb_read.c
 *
 *  Created on: Jan 28, 2020
 *      Author: stoja
 */

#include "typedef.h"
#include "tasks/task_sb_read.h"

/* USER CODE BEGIN Header_vTaskSbRead */
/**
 * @brief Function implementing the task_sb_read thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_vTaskSbRead */
void vTaskSbRead(void *argument) {
	baro_data b1, b2, b3;
	imu_data i1, i2, i3;
	sb_data rcv_sb1, rcv_sb2, rcv_sb3;
	for (;;) {
		/* acquire data from sb1 */
		rcv_sb1.imu = i1;
		rcv_sb1.baro = b1;
		/* acquire data from sb2 */
		rcv_sb2.imu = i2;
		rcv_sb2.baro = b2;
		/* acquire data from sb3 */
		rcv_sb3.imu = i3;
		rcv_sb3.baro = b3;

		if (osMutexAcquire(sb_mutex, SB_MUTEX_TIMEOUT) == osOK) {
			sb1 = rcv_sb1;
			sb2 = rcv_sb2;
			sb3 = rcv_sb3;
			osMutexRelease(sb_mutex);
		}
		osDelay(1);
	}
}
