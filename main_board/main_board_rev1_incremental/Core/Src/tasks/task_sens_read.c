/*
 * task_sens_read.c
 *
 *  Created on: Dec 26, 2019
 *      Author: stoja
 */

#include "tasks/task_sens_read.h"
#include "typedef.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"
#include "stm32f7xx_hal.h"

void vTaskSensRead(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;

	char output_buffer[30] = { 0 };

	//uint8_t baro_data[12] = { 0 };

	baro_data last_baro_data = { 0 };

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / SENSOR_READ_FREQUENCY;
	for (;;) {
		tick_count += tick_update;

		HAL_SPI_Receive(&hspi3, (uint8_t*) &last_baro_data, sizeof(baro_data),
		BARO_READ_TIMEOUT);

// 		sprintf(output_buffer, "P: %d; T: %d; t: %lu\n", 10,
//				15, tick_count);
//
//		CDC_Transmit_FS((uint8_t*) output_buffer, strlen(output_buffer));

		osDelayUntil(tick_count);
	}
}
