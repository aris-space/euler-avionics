/*
 * task_motor_control.c
 *
 *  Created on: May 23, 2020
 *      Author: Jonas
 */

#include "tasks/task_motor_control.h"

void testairbrakes();


void vTaskMotorCont(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;

	osStatus_t motor_status = osOK;

	/* Telemetry Command and FSM State */
	command_e telemetry_command = IDLE_COMMAND;
	flight_phase_detection_t flight_phase_detection = { 0 };
	flight_phase_detection.flight_phase = IDLE;
	flight_phase_detection.mach_number = SUBSONIC;


	/* Initialisation */
	//int8_t position_mode = 0x08;
	/* Profile Position Mode */
	int8_t position_mode = 0x01;
	int32_t PPM_velocity = 2000;
	int32_t PPM_acceleration = 100000;
	int32_t PPM_deceleration = 100000;


	/* Controller Variables */
	int32_t controller_actuation = 0;

	/* Task Variables */
	int32_t desired_motor_position = 0;
	int32_t measured_motor_position = 0;

//	while(EnableMotor() != osOK){
//		osDelay(1000);
//		break;
//	};
	EnableMotor();

	SetPositionMode(position_mode);
//	while(SetPositionMode(position_mode) != osOK){
//		osDelay(1000);
//		break;
//	};

	if (position_mode == 0x01) {
		motor_status = ConfigurePPM(PPM_velocity, PPM_acceleration, PPM_deceleration);
	}


	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / MOTOR_TASK_FREQUENCY;

	for (;;) {
		tick_count += tick_update;

		/* Read Current Motor Position */
		motor_status = GetPosition(&measured_motor_position);

		/* Read Telemetry Command */
		ReadMutex(&command_mutex, &global_telemetry_command, &telemetry_command, sizeof(global_telemetry_command));

		/* Read FSM State */
		ReadMutex(&fsm_mutex, &global_flight_phase_detection, &flight_phase_detection, sizeof(global_flight_phase_detection));

		/* Read in Current Controller Output */
		ReadMutex(&controller_mutex, &controller_output_global, &controller_actuation, sizeof(controller_actuation));

		/* Transform 0-1 Controller output to controller output of motor */
		desired_motor_position = (int32_t)(((float)controller_actuation)/1000*(-140));

		/* Check Bounds */
		if(desired_motor_position > -2){
			desired_motor_position = -2;
		}

		if(desired_motor_position < -135){
			desired_motor_position = -135;
		}

		/* If we are in IDLE, THRUSTING or DESCENDING
		 * the Motor is not allowed to Move!
		 */
		if(flight_phase_detection.flight_phase == COASTING){
			/* Move the Motor */
			MoveToPositionPPM(desired_motor_position);
		}
		else{
		//	MoveToPositionPPM(0);
		}

		/* Airbrake Test if telemetry command is given and we are in idle state */
		if(flight_phase_detection.flight_phase == IDLE && telemetry_command == AIRBRAKE_TEST_COMMAND){
			testairbrakes();
		}

		MoveToPositionPPM(-150);

		/* Log Motor Position and Desired Motor Position */
		logMotor(osKernelGetTickCount(), desired_motor_position, measured_motor_position);


		osDelayUntil(tick_count);
	}
}

void testairbrakes(){
	MoveToPositionPPM(-150);
	osDelay(1000);
	MoveToPositionPPM(0);
}


