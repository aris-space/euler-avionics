/*
 * task_xbee.c
 *
 *  Created on: Jun 15, 2020
 *      Author: Jonas
 */
#include "tasks/task_xbee.h"

command_xbee_t local_command_xbee;

command_e local_command;
bool new_command = false;

void vTaskXbee(void *argument) {
	/* For periodic update */
	uint32_t tick_count, tick_update;

	/* local Data */
	state_est_data_t state_est_data;
	int32_t controller_output;

	/* Telemetry struct */
	telemetry_t telemetry_send = { 0 };
	telemetry_send.flight_phase = IDLE;
	telemetry_send.mach_regime = SUBSONIC;


	/* Command struct */
	local_command = IDLE_COMMAND;
	local_command_xbee = IDLE_XBEE_DATA;


	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / XBEE_SAMPLING_FREQ;

	HAL_UART_Receive_IT(&huart7, (uint8_t*) &local_command, sizeof(local_command));

	while (1) {
		/* Tick Update */
		tick_count += tick_update;

		/* Write command in Global Command Variable */
		if(AcquireMutex(&command_mutex) == osOK){
			global_telemetry_command = local_command;
			ReleaseMutex(&command_mutex);
		}
		if(new_command){
			local_command = IDLE_COMMAND;
			new_command = false;
		}

		/* Read Sensor Board Data */
		ReadMutex(&sb1_mutex, &sb1_baro, &telemetry_send.sb1.baro, sizeof(sb1_baro));
		ReadMutex(&sb1_mutex, &sb1_imu, &telemetry_send.sb1.imu, sizeof(sb1_imu));
		ReadMutex(&sb2_mutex, &sb2_baro, &telemetry_send.sb2.baro, sizeof(sb2_baro));
		ReadMutex(&sb2_mutex, &sb2_imu, &telemetry_send.sb2.imu, sizeof(sb2_imu));
		ReadMutex(&sb3_mutex, &sb3_baro, &telemetry_send.sb3.baro, sizeof(sb3_baro));
		ReadMutex(&sb3_mutex, &sb3_imu, &telemetry_send.sb3.imu, sizeof(sb3_imu));

		/* Read Control Data*/
		ReadMutex(&state_est_mutex, &state_est_data_global, &state_est_data, sizeof(state_est_data));

		ReadMutex(&controller_mutex, &controller_output_global, &controller_output, sizeof(controller_output));

		ReadMutex(&fsm_mutex, &global_flight_phase_detection.flight_phase, &telemetry_send.flight_phase, sizeof(global_flight_phase_detection.flight_phase));
		ReadMutex(&fsm_mutex, &global_flight_phase_detection.mach_regime, &telemetry_send.mach_regime, sizeof(global_flight_phase_detection.mach_regime));


		telemetry_send.height = state_est_data.position_world[2];
		telemetry_send.velocity = state_est_data.velocity_world[2];
		telemetry_send.ts = osKernelGetTickCount();

		/* Send to Xbee module */
		HAL_UART_Transmit(&huart7, (uint8_t*) &telemetry_send, sizeof(telemetry_send), HAL_MAX_DELAY);



		/* Sleep */
		osDelayUntil(tick_count);
	}
}

void HAL_UART_RxCptlCallback(UART_HandleTypeDef *huart){
	HAL_UART_Receive_IT(&huart7, (uint8_t*) &local_command_xbee, sizeof(local_command_xbee));
	for(int i = 0; i<3; i++){
		if(local_command_xbee.command[i] != local_command_xbee.command[i+1]){
			break;
		}
		if(i == 2){
			local_command = local_command_xbee.command[0];
			new_command = true;
		}
	}

}
