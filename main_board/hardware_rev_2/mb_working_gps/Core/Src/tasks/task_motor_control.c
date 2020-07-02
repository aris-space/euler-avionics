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


	/* Initialisation */
	//int8_t position_mode = 0x08;
	/* Profile Position Mode */
	int8_t position_mode = 0x01;
	int32_t PPM_velocity = 4000;
	int32_t PPM_acceleration = 100000;
	int32_t PPM_deceleration = 100000;


	/* Controller Variables */
	int32_t controller_actuation = 0;
	int32_t controller_actuation_dummy = 0;

	/* Debugging Variables */
	int8_t counter = 0;
	int32_t desired_position = 0;
	int32_t measured_position = 0;

	while(EnableMotor() != osOK){
		osDelay(1000);
	};

	while(SetPositionMode(position_mode) != osOK){
		osDelay(1000);
	};

	if (position_mode == 0x01) {
		motor_status = ConfigurePPM(PPM_velocity, PPM_acceleration, PPM_deceleration);
	}


	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / MOTOR_TASK_FREQUENCY;

	for (;;) {
		tick_count += tick_update;
		motor_status = GetPosition(&measured_position);


		if (osMutexGetOwner(controller_mutex) == NULL) {
			controller_actuation_dummy = controller_output_global;
			if (osMutexGetOwner(controller_mutex) == NULL) {
				controller_actuation = controller_actuation_dummy;
			}
		}
		desired_position = (int32_t)(((float)controller_actuation)/1000*(-140));

		if(desired_position > -10){
			desired_position = -10;
		}

		if(desired_position < -135){
			desired_position = -135;
		}

		MoveToPositionPPM(desired_position);

//		if(counter < 25){
//			desired_position = desired_position - 2;
//		}
//		else if(counter < 50){
//			desired_position = desired_position + 2;
//		}
//		else{
//			counter = 0;
//			desired_position = desired_position - 2;
//		}
//		counter++;
//		if (counter == 0){
//			desired_position = -50;
//			motor_status = MoveToPositionPPM(desired_position);
//			counter = 1;
//		}
//		else {
//			desired_position = -5;
//			motor_status = MoveToPositionPPM(desired_position);
//			counter = 0;
//		}

		logMotor(osKernelGetTickCount(), desired_position, measured_position);


		osDelayUntil(tick_count);
	}
}


