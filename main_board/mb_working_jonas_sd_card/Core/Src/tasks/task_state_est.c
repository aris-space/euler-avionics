/*
 * task_state_est.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "tasks/task_state_est.h"


void vTaskStateEst(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;


	/* Initialise Variables */
	env_t env;
	init_env(&env);

	state_est_meas_t state_est_meas = { 0 };
	state_est_meas_t state_est_meas_prior = { 0 };
	uint32_t Placeholder_timestamps[3] = { 0 };
	float Placeholder_measurement[2] = { 0 };

	kf_state_t kf_state;
	reset_kf_state(&kf_state);

	extrapolation_rolling_memory_t extrapolation_rolling_memory = {0};
	extrapolation_rolling_memory.memory_length = 0;

	flight_phase_detection_t dummy_flight_phase_detection = { 0 };
	flight_phase_detection_t flight_phase_detection = { 0 };
	reset_flight_phase_detection(&flight_phase_detection);
	reset_flight_phase_detection(&dummy_flight_phase_detection);

	select_noise_models(&kf_state, &flight_phase_detection, &env, &extrapolation_rolling_memory);


	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / STATE_ESTIMATION_FREQUENCY;
	for (;;) {
		tick_count += tick_update;

		/* Acquire the Sensor data */
		/* Sensor Board 1 */
		if (osMutexGetOwner(sb1_mutex) == NULL) {
			Placeholder_measurement[0] = (float) (sb1_data.baro.pressure);
			Placeholder_timestamps[0] = sb1_data.baro.ts;
			Placeholder_measurement[1] = ((float) (sb1_data.imu.acc_z)) / 1024;
			Placeholder_timestamps[1] = sb1_data.imu.ts;
			Placeholder_measurement[2] = ((float) (sb1_data.baro.temperature)) / 100;

			if (osMutexGetOwner(sb1_mutex) == NULL) {
				state_est_meas.baro_data[0].pressure = Placeholder_measurement[0];
				state_est_meas.baro_data[0].temperature = Placeholder_measurement[2];
				state_est_meas.baro_data[0].ts = Placeholder_timestamps[0];

				state_est_meas.imu_data[0].acc_x = Placeholder_measurement[1] * GRAVITATION;
				state_est_meas.imu_data[0].ts = Placeholder_timestamps[1];
			}
		}

		/* Sensor Board 2 */
		if (osMutexGetOwner(sb1_mutex) == NULL) {
			Placeholder_measurement[0] = (float) (sb2_data.baro.pressure);
			Placeholder_timestamps[0] = sb2_data.baro.ts;
			Placeholder_measurement[1] = ((float) (sb2_data.imu.acc_z)) / 1024;
			Placeholder_timestamps[1] = sb2_data.imu.ts;
			Placeholder_measurement[2] = ((float) (sb2_data.baro.temperature)) / 100;

			if (osMutexGetOwner(sb1_mutex) == NULL) {
				state_est_meas.baro_data[1].pressure = Placeholder_measurement[0];
				state_est_meas.baro_data[1].temperature = Placeholder_measurement[2];
				state_est_meas.baro_data[1].ts = Placeholder_timestamps[0];

				state_est_meas.imu_data[1].acc_x = Placeholder_measurement[1] * GRAVITATION;
				state_est_meas.imu_data[1].ts = Placeholder_timestamps[1];
			}
		}

		/* Sensor Board 3 */
		if (osMutexGetOwner(sb1_mutex) == NULL) {
			Placeholder_measurement[0] = (float)(sb3_data.baro.pressure);
			Placeholder_timestamps[0] = sb3_data.baro.ts;
			Placeholder_measurement[1] = ((float)(sb3_data.imu.acc_z)) / 1024;
			Placeholder_timestamps[1] = sb3_data.imu.ts;
			Placeholder_measurement[2] = ((float)(sb3_data.baro.temperature)) / 100;

			if (osMutexGetOwner(sb1_mutex) == NULL) {
				state_est_meas.baro_data[2].pressure = Placeholder_measurement[0];
				state_est_meas.baro_data[2].temperature = Placeholder_measurement[2];
				state_est_meas.baro_data[2].ts = Placeholder_timestamps[0];

				state_est_meas.imu_data[2].acc_x = Placeholder_measurement[1] * GRAVITATION;
				state_est_meas.imu_data[2].ts = Placeholder_timestamps[1];
			}
		}

		/* get new Phase Detection*/
		if(osMutexGetOwner(fsm_mutex) == NULL){
			dummy_flight_phase_detection = global_flight_phase_detection;
			if(osMutexGetOwner(fsm_mutex) == NULL){
				flight_phase_detection = dummy_flight_phase_detection;
			}
		}

		/* process measurements */
		process_measurements(tick_count, &kf_state, &state_est_meas, &state_est_meas_prior, &env, &extrapolation_rolling_memory);

		/* select noise models (dependent on detected flight phase and updated temperature in environment) */
		select_noise_models(&kf_state, &flight_phase_detection, &env, &extrapolation_rolling_memory);

		/* Start Kalman Update */

		/* Prediction Step */
		kf_prediction(&kf_state);

		/* update Step */
		if (kf_state.num_z_active > 0) {
			select_kf_observation_matrices(&kf_state);
			kf_update(&kf_state);
		}
		else
		{
			memcpy(kf_state.x_est, kf_state.x_priori, sizeof(kf_state.x_priori));
		}

		/* set measurement prior to measurements from completed state estimation step */
		memcpy(&state_est_meas_prior, &state_est_meas, sizeof(state_est_meas));


		/* KALMAN UPDATE FINISHED */
		/* OUTPUT IS x_est */
		if(osMutexAcquire(state_est_mutex, 10) == osOK){
			/* Write into global variable */
			/* TODO: Check correct indexing */
			/* the value is multiplied by 1000 for conversion to int datatype for easy transport
			 * careful in other tasks!
			 */
			update_state_est_data(&state_est_data_global, &kf_state);
			osMutexRelease(state_est_mutex);
		}

		/* Update env for FSM */
		if(osMutexAcquire(environment_mutex, 10) == osOK){
			global_env = env;
			osMutexRelease(environment_mutex);
		}

		/* Write to logging system */
		logEstimatorVar(osKernelGetTickCount(), state_est_data_global);

		/* TODO: Check if the state estimation can do this for the given frequency */

		osDelayUntil(tick_count);
	}
}
