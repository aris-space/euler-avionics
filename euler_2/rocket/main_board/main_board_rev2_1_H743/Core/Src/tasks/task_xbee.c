/*
 * task_xbee.c
 *
 *  Created on: Jun 15, 2020
 *      Author: Jonas
 */
#include "tasks/task_xbee.h"
#include "Util/RS.h"

command_xbee_t local_command_xbee;

command_e local_command_rx;
command_e local_command;
uint8_t buffer[1];
bool new_command = false;

int uart_counter = 0;

void vTaskXbee(void *argument) {
	/* For periodic update */
	uint32_t tick_count, tick_update_slow, tick_update_fast;

	/* local Data */
	state_est_data_t state_est_data;
	sb_data_t local_sb_data;

	/* Telemetry struct */
	telemetry_t telemetry_send = {0};
	telemetry_send.flight_phase = IDLE_T;

	/* Command struct */
	local_command_rx = IDLE_COMMAND;
	local_command = IDLE_COMMAND;

	/* Reed Solomon Initialisation */
	int16_t alpha_to[nn + 1];
	int16_t index_of[nn + 1];
	int16_t gg[nn - kk + 1];
	int16_t pp[mm + 1];
	int16_t recd[nn];
	int16_t bb[nn - kk];
	int16_t data[kk];
	int16_t recd_compact[nn-kk+kk2];


	/* set irreducible polynomial */
	set_irr_poly(pp);

	/* generate the Galois Field GF(2**mm) */
	generate_gf(pp, index_of, alpha_to);

	/* compute the generator polynomial for this RS code */
	gen_poly(gg, alpha_to, index_of);


	osDelay(700);


	/* Infinite loop */
	tick_update_slow = osKernelGetTickFreq() / XBEE_SAMPLING_FREQ;
	tick_update_fast = osKernelGetTickFreq() / XBEE_SAMPLING_FREQ_HIGH;
	bool fast_sampling = true;
	tick_count = osKernelGetTickCount();

	while (1) {
		/* Tick Update */
		if (fast_sampling) {
			tick_count += tick_update_fast;
		} else {
			tick_count += tick_update_slow;
		}
		/* Read Command */
		//		HAL_UART_Receive_IT(&huart7, (uint8_t*) &local_command_rx, 1);
		//		UsbPrint("[Telemetry] ts: %u, Received Commmand: %u, Rx_buffer;
		//%u\n", 				telemetry_send.ts, local_command,
		// local_command_rx);
		if (acquire_mutex(&command_mutex) == osOK) {
			global_telemetry_command = local_command;
			release_mutex(&command_mutex);
		}

		/* Check if we need to go to fast sampling */
		if (local_command == TELEMETRY_HIGH_SAMPLING) {
			fast_sampling = true;
		}

		/* Check if we need to go to low sampling */
		if (local_command == TELEMETRY_LOW_SAMPLING) {
			fast_sampling = false;
		}

		/* Go Back to Low Sampling if we are in Touchdown */
		if (telemetry_send.flight_phase == TOUCHDOWN_T) {
			fast_sampling = false;
		}

		/* reset command */
		if (new_command) {
			local_command = IDLE_COMMAND;
			new_command = false;
		}

		/* Read Sensor Board Data */
		read_mutex(&sb1_mutex, &sb1_baro, &local_sb_data.baro, sizeof(sb1_baro));
		read_mutex(&sb1_mutex, &sb1_imu_1, &local_sb_data.imu_1, sizeof(sb1_imu_1));

		telemetry_send.sb_data.pressure = local_sb_data.baro.pressure;
		telemetry_send.sb_data.temperature = local_sb_data.baro.temperature;
		telemetry_send.sb_data.acc_x = local_sb_data.imu_1.acc_x;
		telemetry_send.sb_data.acc_y = local_sb_data.imu_1.acc_y;
		telemetry_send.sb_data.acc_z = local_sb_data.imu_1.acc_z;
		telemetry_send.sb_data.gyro_x = local_sb_data.imu_1.gyro_x;
		telemetry_send.sb_data.gyro_y = local_sb_data.imu_1.gyro_y;
		telemetry_send.sb_data.gyro_z = local_sb_data.imu_1.gyro_z;

		/* Read Control Data*/
		read_mutex(&state_est_mutex, &state_est_data_global, &state_est_data,
				sizeof(state_est_data));

		read_mutex(&airbrake_ext_mutex, &global_airbrake_ext_meas,
				&telemetry_send.airbrake_extension,
				sizeof(airbrake_ext_mutex));

		read_mutex(&fsm_mutex, &global_flight_phase_detection.flight_phase,
				&telemetry_send.flight_phase,
				sizeof(global_flight_phase_detection.flight_phase));

		/* read GPS */
		read_mutex(&gps_mutex, &globalGPS, &telemetry_send.gps, sizeof(globalGPS));

		/* read Battery */
		read_mutex(&battery_mutex, &global_battery_data, &telemetry_send.battery,
				sizeof(global_battery_data));

		telemetry_send.height = state_est_data.position_world[2];
		telemetry_send.velocity = state_est_data.velocity_world[2];
		telemetry_send.ts = osKernelGetTickCount();


		/*=============Encoding Part of Reed Solomon =============*/

		/* convert struct to coefficients of polynomial */
		struct_to_poly(telemetry_send, data);

		/* encode data[] to produce parity in bb[].  Data input and parity output
		   is in polynomial form
		 */
		encode_rs(bb, index_of, alpha_to, gg, data);

		/* put the transmitted codeword, made up of data plus parity, in recd[] */
		for (int i = 0; i < nn - kk; i++) recd[i] = bb[i];

		/* compress data for transmission */
		compress_data(recd, recd_compact);

		/* Send recd_compact */

		/* Send to Xbee module */
		HAL_UART_Transmit_DMA(&huart7, (uint8_t*) recd_compact,
				sizeof(recd_compact));

		uart_counter = 0;
		/* Sleep */
		osDelayUntil(tick_count);
	}
}

/* Callback function onto the telemetry receive. If we receive 4 Times
 * Exactly the same command we received an actual command and need to
 * propagate it into the system
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == &huart7) {
		static uint8_t buffer[4];
		buffer[uart_counter] = local_command_rx;
		uart_counter++;
		if (uart_counter == 4) {
			uint8_t succesful = 1;
			for (int i = 1; i < 4; i++)
				if (buffer[0] != buffer[i]) succesful = 0;
			uart_counter = 0;
			if (succesful) {
				new_command = true;
				local_command = local_command_rx;
			}
		}

		HAL_UART_Receive_IT(huart, (uint8_t *)&local_command_rx, 1);
	}
}
