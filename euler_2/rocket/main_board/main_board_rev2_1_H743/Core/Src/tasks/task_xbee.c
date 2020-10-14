/*
 * task_xbee.c
 *
 *  Created on: Jun 15, 2020
 *      Author: Jonas
 */
#include "tasks/task_xbee.h"
#include "Util/RS.h"

#if (configUSE_TRACE_FACILITY == 1)
	traceString xb_channel;
#endif

command_xbee_t local_command_xbee;

command_e local_command_rx;
command_e local_command;
uint8_t buffer[1];
bool new_command = false;

int uart_counter = 0;

/* Reed Solomon Initialisation */

DTCM int16_t alpha_to[NN + 1];
DTCM int16_t index_of[NN + 1];
DTCM int16_t gg[NN - KK + 1];
DTCM int16_t pp[MM + 1];
DTCM int16_t recd[NN];
DTCM int16_t bb[NN - KK];
DTCM int16_t tele_data[KK];
DTCM int16_t recd_compact[NN-KK+KK2];

uint8_t transmission_data[NN-KK+KK2];


void vTaskXbee(void *argument) {

#if (configUSE_TRACE_FACILITY == 1)
  xb_channel = xTraceRegisterString("Xbee Channel");
#endif

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




	/* set irreducible polynomial */
	set_irr_poly(pp);

	/* generate the Galois Field GF(2**mm) */
	generate_gf(pp, index_of, alpha_to);

#if (configUSE_TRACE_FACILITY == 1)
                vTracePrint(xb_channel, "GF GENERATED");
#endif

	/* compute the generator polynomial for this RS code */
	gen_poly(gg, alpha_to, index_of);

#if (configUSE_TRACE_FACILITY == 1)
                vTracePrint(xb_channel, "POLY GENERATED");
#endif

	osDelay(700);


	/* Infinite loop */
	tick_update_slow = osKernelGetTickFreq() / XBEE_SAMPLING_FREQ;
	tick_update_fast = osKernelGetTickFreq() / XBEE_SAMPLING_FREQ_HIGH;
	bool fast_sampling = false;
	HAL_UART_Receive_IT(&huart7, (uint8_t *)&local_command_rx, 1);
	tick_count = osKernelGetTickCount();

	telemetry_send.alignment1 = 0xFAFAFAFA;
	telemetry_send.alignment2 = 0xFEFEFEFE;
	telemetry_send.alignment3 = 0xDCDCDCDC;

	while (1) {
		/* Tick Update */
		if (fast_sampling) {
			tick_count += tick_update_fast;
		} else {
			tick_count += tick_update_slow;
		}

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
		read_mutex(&sb1_mutex, &sb1_global, &local_sb_data, sizeof(sb1_global));

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
				sizeof(telemetry_send.airbrake_extension));

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
		struct_to_poly(telemetry_send, tele_data);

		/* encode data[] to produce parity in bb[].  Data input and parity output
		   is in polynomial form
		 */
		encode_rs(bb, index_of, alpha_to, gg, tele_data);

		/* put the transmitted codeword, made up of data plus parity, in recd[] */
		for (int i = 0; i < KK; i++) recd[i] = tele_data[i];
		for (int i = 0; i < NN-KK; i++) recd[i+KK-1] = bb[i];

		/* compress data for transmission */
		compress_data(recd, recd_compact);

		for (int i= 0; i < sizeof(recd_compact)/2; i++){
			transmission_data[i] = (uint8_t)recd_compact[i];
		}

#if (configUSE_TRACE_FACILITY == 1)
                vTracePrint(xb_channel, "compress_data done");
#endif

		/* Send recd_compact */

		/* Send to Xbee module */
		HAL_UART_Transmit_DMA(&huart7, transmission_data,
				sizeof(transmission_data));

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
