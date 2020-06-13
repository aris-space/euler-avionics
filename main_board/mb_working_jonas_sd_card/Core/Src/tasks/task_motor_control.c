/*
 * task_motor_control.c
 *
 *  Created on: May 23, 2020
 *      Author: Jonas
 */

#include "tasks/task_motor_control.h"


void vTaskMotorCont(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;

	osStatus_t motor_status = osOK;

	uint32_t desired_position = 0;
	int32_t measured_position = 0;

	while(EnableMotor() != osOK){};

	while(SetPositionMode() != osOK){};


	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / MOTOR_TASK_FREQUENCY;

	for (;;) {
		tick_count += tick_update;
		motor_status = GetPosition(&measured_position);
		motor_status = MoveToPosition(desired_position);
		desired_position = desired_position + 50;
		if(desired_position > 500){
			desired_position = 0;
		}

		osDelayUntil(tick_count);
	}
}


