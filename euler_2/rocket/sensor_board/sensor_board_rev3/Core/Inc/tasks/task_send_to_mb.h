/*
 * task_send_to_mb.h
 *
 *  Created on: Nov 9, 2019
 *      Author: Jonas
 */

#ifndef INC_TASK_SEND_TO_MB_H_
#define INC_TASK_SEND_TO_MB_H_

/* Include */
#include "cmsis_os.h"
#include "util.h"
#include "main.h"
#include <string.h>
/* Tasks */
void vTaskSendToMb(void *argument);

/* Constants */
#define SPI_TIMEOUT 5
#define MB_SAMPLING_RATE SAMPLING_FREQ

/* extern */
extern baro_data_t baro_data_to_mb;
extern imu_data_t imu_data_1_to_mb;
extern imu_data_t imu_data_2_to_mb;
extern osMutexId_t baro_mutex;
extern osMutexId_t imu_mutex_1;
extern osMutexId_t imu_mutex_2;
extern SPI_HandleTypeDef hspi2;

/* USB Sensor Read In */
//extern osMessageQueueId_t usb_data_queue;
extern osMutexId_t usb_data_mutex;
extern uint8_t usb_data_buffer[64];

#endif /* INC_TASK_SEND_TO_MB_H_ */
