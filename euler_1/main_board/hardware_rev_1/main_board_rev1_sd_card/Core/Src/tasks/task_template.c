/*
 * task_template.c
 *
 *  Created on: Feb 24, 2020
 *      Author: stoja
 */

#include "util.h"

#include "tasks/task_template.h"


void vTaskTemplate(void *argument) {
	uint32_t out_int = 1;
	char* out_str = "template";
	for (;;) {
		UsbPrint("[DBG] Hello from %s task %d!\n", out_str, out_int);
		osDelay(1000);
	}
}
