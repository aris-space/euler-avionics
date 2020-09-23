/*
 * task_flash.c
 *
 *  Created on: Aug 20, 2020
 *      Author: Jonas
 */
#include "tasks/task_flash.h"

void vTaskFlash(void *argument) {
	/* For periodic update */
	uint32_t tick_count, tick_update;
	erase_chip();

	osDelay(500);

	tick_update = osKernelGetTickFreq() / FLASH_SAMPLING_FREQ;
	tick_count = osKernelGetTickCount();

//	uint32_t data_write = 0xFFFF0000;

	test_data_t data_write = { .data = { 0 }, .s = { 'a', 'b', 'c' } };
	test_data_t data_read = { .data = { -1 }, .s = { 'd', 'e', 'f' } };

//	for (int i = 0; i < 5; i++) {
//		data_write.data[i] = i;
//	}

//	uint32_t data_read = 0;

	uint32_t address = 0x0000F300;
	uint32_t start_address = address;
	uint32_t counter = 0;

	while (1) {
		/* Tick Update */
		tick_count += tick_update;

		int32_t chip_id = 0;

		/* WORK */
		osDelay(1);
		chip_id = read_chip_id();
		osDelay(1);
		write_data(address, &data_write, sizeof(data_write));
		osDelay(1);
		read_data(address - sizeof(data_write) + 4, &data_read,
				sizeof(data_read));
//		read_data(address, &data_read, sizeof(data_read));
		UsbPrint(
				"[FLASH]: ID: %ld, D0: %ld, D1: %ld, D2: %ld, D3: %ld, D4: %ld, S: %s\n",
				chip_id, data_read.data[0], data_read.data[1],
				data_read.data[2], data_read.data[3], data_read.data[4],
				data_read.s);
		address += sizeof(data_write) + 4; //0x100

		data_write.data[0]++;
		data_write.data[1] += 2;
		data_write.data[2] += 3;
		data_write.data[3] += 4;
		data_write.data[4] += 5;

		if (data_write.data[1] % 10 == 0) {
			data_write.s[0] = 'x';
			data_write.s[1] = 'y';
			data_write.s[2] = 'z';
		} else {
			data_write.s[0] = 'a';
			data_write.s[1] = 'b';
			data_write.s[2] = 'c';
		}

		if (counter == 10) {
			erase_4KB(start_address);
			counter = 1;
			address = start_address;
		};

		/* Sleep */
		osDelayUntil(tick_count);
	}
}
