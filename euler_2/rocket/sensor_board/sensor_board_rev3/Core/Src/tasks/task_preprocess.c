/*
 * task_preprocess.c
 *
 *  Created on: Nov 8, 2019
 *      Author: Jonas
 */

#include "tasks/task_preprocess.h"

void lp_filter(float registers[], int32_t raw_acc_z, int32_t *filtered_acc_z);


void vTaskPreprocess(void *argument) {
	/* registers for the Filter */
	float registers_imu_1[4] = { 0 };
	float registers_imu_2[4] = { 0 };

	int32_t filtered_acc_z = 0;


	/* Queue data and Status Initialization */
	imu_data_t queue_data_imu_1 = { 0 };
	imu_data_t queue_data_imu_2 = { 0 };



	/* Infinite loop */
	for (;;) {

		/* IMU 1 */
		if (osMessageQueueGet(preprocess_queue_imu_1, &queue_data_imu_1, NULL,
				1) == osOK) {

			/* if not, filter Value */
			//lp_filter(registers_imu_1, queue_data_imu_1.acc_z, &filtered_acc_z);

			/* Write Result into Motherboard Variable if Mutex is available */
			if (osMutexAcquire(imu_mutex_1, IMU_MUTEX_TIMEOUT) == osOK) {
				imu_data_1_to_mb = queue_data_imu_1;
				//				imu_data_to_mb.acc_z = filtered_acc_z; /* Insert filtered data */
				osMutexRelease(imu_mutex_1);
			}
		}

		/* IMU 2 */
		osStatus_t thing = osMessageQueueGet(preprocess_queue_imu_2Handle, &queue_data_imu_2, NULL,
				1);
		if (thing == osOK) {

			//lp_filter(registers_imu_2, queue_data_imu_2.acc_z, &filtered_acc_z);

			/* Write Result into Motherboard Variable if Mutex is available */
			if (osMutexAcquire(imu_mutex_2, IMU_MUTEX_TIMEOUT) == osOK) {
				imu_data_2_to_mb = queue_data_imu_2;
				//				imu_data_to_mb.acc_z = filtered_acc_z; /* Insert filtered data */
				osMutexRelease(imu_mutex_2);
			}
		}

		/* Sleep */
		osDelay(1);
	}
}

void lp_filter(float registers[], int32_t raw_acc_z, int32_t *filtered_acc_z){
	*filtered_acc_z = A0
			* (raw_acc_z + B1 * registers[0] + B2 * registers[1]
			+ B3 * registers[2] + B4 * registers[3]) +
			A1 * registers[0] + A2 * registers[1] + A3 * registers[2]
			+ A4 * registers[3];
	registers[3] = registers[2];
	registers[2] = registers[1];
	registers[1] = registers[0];
	registers[0] = raw_acc_z + B1 * registers[0] + B2 * registers[1]
			+ B3 * registers[2] + B4 * registers[3];
}
