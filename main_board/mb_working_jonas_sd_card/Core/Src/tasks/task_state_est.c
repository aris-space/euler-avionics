/*
 * task_state_est.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "tasks/task_state_est.h"

void reset_ekf_state(ekf_state_t *ekf_state);
void ekf_update(ekf_state_t *ekf_state);
void ekf_prediction(ekf_state_t *ekf_state);



void vTaskStateEst(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;


	/* Initialise Variables */
	env environment;
	init_env(&environment);

	flight_phase_detection_t dummy_flight_phase_detection = { 0 };
	flight_phase_detection_t flight_phase_detection = { 0 };
	reset_flight_phase_detection(&flight_phase_detection);
	reset_flight_phase_detection(&dummy_flight_phase_detection);
	state_est_meas_t measured_data = { 0 };

	/* Initialise States */
	ekf_state_t ekf_state = { 0 };
	reset_ekf_state(&ekf_state);

	/* Initialise placeholder variables for sensor reading */
	float Placeholder_measurement[3] = { 0 };
	uint32_t Placeholder_timestamps[2] = { 0 };



	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / STATE_ESTIMATION_FREQUENCY;
	for (;;) {
		tick_count += tick_update;

		/* Acquire the Sensor data */
		/* Sensor Board 1 */
		if(osMutexGetOwner(sb1_mutex) == NULL){
			Placeholder_measurement[0] = (float) sb1_data.baro.pressure;
			Placeholder_timestamps[0] = sb1_data.baro.ts;
			Placeholder_measurement[1] = (float) sb1_data.imu.acc_z;
			Placeholder_timestamps[1] = sb1_data.imu.ts;
			Placeholder_measurement[2] = (float) sb1_data.baro.temperature;

			if(osMutexGetOwner(sb1_mutex) == NULL){
				measured_data.baro_data[0].pressure = Placeholder_measurement[0];
				measured_data.baro_data[0].temperature = Placeholder_measurement[2];
				measured_data.baro_data[0].ts = Placeholder_timestamps[0];

				measured_data.imu_data[0].acc_z = Placeholder_measurement[1];
				measured_data.imu_data[0].ts = Placeholder_timestamps[1];
			}
		}

		/* Sensor Board 2 */
		if(osMutexGetOwner(sb2_mutex) == NULL){
			Placeholder_measurement[0] = (float) sb2_data.baro.pressure;
			Placeholder_timestamps[0] = sb2_data.baro.ts;
			Placeholder_measurement[1] = (float) sb2_data.imu.acc_z;
			Placeholder_timestamps[1] = sb2_data.imu.ts;
			Placeholder_measurement[2] = (float) sb2_data.baro.temperature;
			if(osMutexGetOwner(sb2_mutex) == NULL){
				measured_data.baro_data[1].pressure = Placeholder_measurement[0];
				measured_data.baro_data[1].temperature = Placeholder_measurement[2];
				measured_data.baro_data[1].ts = Placeholder_timestamps[0];

				measured_data.imu_data[1].acc_z = Placeholder_measurement[1];
				measured_data.imu_data[1].ts = Placeholder_timestamps[1];

			}
		}

		/* Sensor Board 3 */
		if(osMutexGetOwner(sb3_mutex) == NULL){
			Placeholder_measurement[0] = (float) sb3_data.baro.pressure;
			Placeholder_timestamps[0] = sb3_data.baro.ts;
			Placeholder_measurement[1] = (float) sb3_data.imu.acc_z;
			Placeholder_timestamps[1] = sb3_data.imu.ts;
			Placeholder_measurement[2] = (float) sb2_data.baro.temperature;
			if(osMutexGetOwner(sb3_mutex) == NULL){
				measured_data.baro_data[2].pressure = Placeholder_measurement[0];
				measured_data.baro_data[2].temperature = Placeholder_measurement[2];
				measured_data.baro_data[2].ts = Placeholder_timestamps[0];

				measured_data.imu_data[2].acc_z = Placeholder_measurement[1];
				measured_data.imu_data[2].ts = Placeholder_timestamps[1];
			}
		}

		/* get new Phase Detection*/
		if(osMutexGetOwner(fsm_mutex) == NULL){
			dummy_flight_phase_detection = global_flight_phase_detection;
			if(osMutexGetOwner(fsm_mutex) == NULL){
				flight_phase_detection = dummy_flight_phase_detection;
			}
		}

		/* TODO: get U from Controller Task */

		/* TODO: Preprocessing of data with timestep etc... */


		/* End TODO */

		/* TODO Get current flight Phase from Global Variable */

		/* Start Kalman Update */

		/* Prediction Step */
		ekf_prediction(&ekf_state);

		/* update Step */
		ekf_update(&ekf_state);


		/* KALMAN UPDATE FINISHED */
		/* OUTPUT IS x_est */
		if(osMutexAcquire(state_est_mutex, 10) == osOK){
			/* Write into global variable */
			/* TODO: Check correct indexing */
			/* the value is multiplied by 1000 for conversion to int datatype for easy transport
			 * careful in other tasks!
			 */
			state_est_data.position_world[2] = (int32_t)(ekf_state.x_est[0]*1000);
			state_est_data.velocity_rocket[0] = (int32_t)(ekf_state.x_est[1]*1000);
			state_est_data.velocity_world[2] = (int32_t)(ekf_state.x_est[1]*1000);
			state_est_data.acceleration_rocket[0] = (int32_t)(ekf_state.u[0]*1000);
			state_est_data.acceleration_rocket[2] = (int32_t)(ekf_state.u[0]*1000);
			osMutexRelease(state_est_mutex);
		}

		/* Update Environment for FSM */
		if(osMutexAcquire(environment_mutex, 10) == osOK){
			global_env = environment;
			osMutexRelease(environment_mutex);
		}

		/* Write to logging system */
		logEstimatorVar(osKernelGetTickCount(), state_est_data);

		/* TODO: Check if the state estimation can do this for the given frequency */

		osDelayUntil(tick_count);
	}
}


void reset_ekf_state(ekf_state_t *ekf_state){

	float A_init[NUMBER_STATES][NUMBER_STATES] = {{0, 1, 0}, {0, 0, 1}, {0, 0, 0}};
	float B_init[NUMBER_STATES][NUMBER_INPUTS] = {{0}, {1}, {0}};
	float G_init[NUMBER_STATES][NUMBER_NOISE] = {{0}, {1}, {0}};
	float H_init[NUMBER_SENSOR][NUMBER_STATES] = {{1, 0, 0},{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}};

	float x_est_init[NUMBER_STATES] = {0, 0, 0};
	float P_est_init[NUMBER_STATES][NUMBER_STATES] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

	memcpy(ekf_state->H, H_init, sizeof(H_init));
	memcpy(ekf_state->Ad, A_init, sizeof(A_init));
	memcpy(ekf_state->Bd, B_init, sizeof(B_init));
    memcpy(ekf_state->Gd, G_init, sizeof(G_init));
    memcpy(ekf_state->x_est, x_est_init, sizeof(x_est_init));
    memcpy(ekf_state->P_est, P_est_init, sizeof(P_est_init));

    memset(ekf_state->Q, 0, NUMBER_NOISE*NUMBER_NOISE*sizeof(ekf_state->Q[0][0]));
    memset(ekf_state->R, 0, NUMBER_SENSOR*NUMBER_SENSOR*sizeof(ekf_state->R[0][0]));

    transpose(NUMBER_STATES, NUMBER_STATES, ekf_state->Ad, ekf_state->Ad_T);
    transpose(NUMBER_STATES, NUMBER_NOISE, ekf_state->Gd, ekf_state->Gd_T);
    transpose(NUMBER_SENSOR, NUMBER_STATES, ekf_state->H, ekf_state->H_T);
}

void ekf_prediction(ekf_state_t *ekf_state){
    /* Prediction Step */
    /* Calculation of x_priori */
    matvecprod(NUMBER_STATES, NUMBER_STATES, ekf_state->Ad, ekf_state->x_est, ekf_state->x_priori, true);
    matvecprod(NUMBER_STATES, NUMBER_INPUTS, ekf_state->Bd, ekf_state->u, ekf_state->x_priori, false);

    /* Calculation of P_priori */
    /* P_priori = Ad * P_est_prior * Ad_T + Gd * Q * Gd_T */
    matmul(NUMBER_STATES, NUMBER_STATES, NUMBER_STATES, ekf_state->Ad, ekf_state->P_est, ekf_state->Placeholder_Ad_mult_P_est, true);
    matmul(NUMBER_STATES, NUMBER_NOISE, NUMBER_NOISE, ekf_state->Gd, ekf_state->Q, ekf_state->Placeholder_Gd_mult_Q, true);

    matmul(NUMBER_STATES, NUMBER_STATES, NUMBER_STATES, ekf_state->Placeholder_Ad_mult_P_est, ekf_state->Ad_T, ekf_state->P_priori, true);
    matmul(NUMBER_STATES, NUMBER_NOISE, NUMBER_STATES, ekf_state->Placeholder_Gd_mult_Q, ekf_state->Gd_T, ekf_state->P_priori, false);
}

void ekf_update(ekf_state_t *ekf_state) {
    /* Update Step */
    /* y = z - H * x_priori */
    matvecprod(NUMBER_SENSOR, NUMBER_STATES, ekf_state->H, ekf_state->x_priori, ekf_state->y, true);
    vecsub(NUMBER_SENSOR, ekf_state->z, ekf_state->y, ekf_state->y);

    /* S = H*P*H_T + R */
    matmul(NUMBER_SENSOR, NUMBER_STATES, NUMBER_STATES, ekf_state->H, ekf_state->P_priori, ekf_state->Placeholder_H_mult_P_priori, true);
    matmul(NUMBER_SENSOR, NUMBER_STATES, NUMBER_SENSOR, ekf_state->Placeholder_H_mult_P_priori, ekf_state->H_T, ekf_state->S, true);
    matadd(NUMBER_SENSOR,  NUMBER_SENSOR, ekf_state->S, ekf_state->R, ekf_state->S);

    /* Calculate Pseudoinverse of covariance innovation */
    memset(ekf_state->S_inv, 0, NUMBER_SENSOR*NUMBER_SENSOR*sizeof(ekf_state->S_inv[0][0]));
    pinv(NUMBER_SENSOR, LAMBDA, ekf_state->S, ekf_state->S_inv);

    /* K  = P_priori * H_T * S_inv */
    matmul(NUMBER_STATES, NUMBER_STATES, NUMBER_SENSOR, ekf_state->P_priori, ekf_state->H_T, ekf_state->Placeholder_P_priori_mult_H_T, true);
    matmul(NUMBER_STATES, NUMBER_SENSOR, NUMBER_SENSOR, ekf_state->Placeholder_P_priori_mult_H_T, ekf_state->S_inv, ekf_state->K, true);

    /* x_est = x_priori + K*y */
    matvecprod(NUMBER_STATES, NUMBER_SENSOR, ekf_state->K, ekf_state->y, ekf_state->x_est, true);
    vecadd(NUMBER_STATES, ekf_state->x_priori, ekf_state->x_est, ekf_state->x_est);


    /* P_est = (eye(NUMBER_STATES) - K*H)*P_priori */
    eye(NUMBER_STATES, ekf_state->Placeholder_eye);
    matmul(NUMBER_STATES, NUMBER_SENSOR, NUMBER_STATES, ekf_state->K, ekf_state->H, ekf_state->Placeholder_K_mult_H, true);
    matsub(NUMBER_STATES, NUMBER_STATES, ekf_state->Placeholder_eye, ekf_state->Placeholder_K_mult_H, ekf_state->Placeholder_P_est);
    matmul(NUMBER_STATES, NUMBER_STATES,  NUMBER_STATES, ekf_state->Placeholder_P_est, ekf_state->P_priori, ekf_state->P_est, true);
}
