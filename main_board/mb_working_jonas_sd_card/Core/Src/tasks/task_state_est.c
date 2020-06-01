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
void select_ekf_observation_matrices(ekf_state_t *ekf_state);

void process_measurements(ekf_state_t *ekf_state, state_est_meas_t *state_est_meas, state_est_meas_t *state_est_meas_prior, env *env);
void update_state_est_data(state_est_data_t *state_est_data, ekf_state_t *ekf_state);
void select_noise_models(ekf_state_t *ekf_state, flight_phase_detection_t *flight_phase_detection, env *env);
void sensor_elimination_by_stdev(int32_t n, float measurements[n], bool measurement_active[n]);




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
	state_est_meas_t state_est_meas = { 0 };
	state_est_meas_t state_est_meas_prior = { 0 };

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
		if (osMutexGetOwner(sb1_mutex) == NULL) {
			Placeholder_measurement[0] = (float) (sb1_data.baro.pressure);
			Placeholder_timestamps[0] = sb1_data.baro.ts;
			Placeholder_measurement[1] = ((float) (sb1_data.imu.acc_z)) / 1024;
			Placeholder_timestamps[1] = sb1_data.imu.ts;
			Placeholder_measurement[2] = ((float) (sb1_data.baro.temperature)) / 100;

			if (osMutexGetOwner(sb1_mutex) == NULL) {
				state_est_meas.baro_state_est[0].pressure = Placeholder_measurement[0];
				state_est_meas.baro_state_est[0].temperature = Placeholder_measurement[2];
				state_est_meas.baro_state_est[0].ts = Placeholder_timestamps[0];

				state_est_meas.imu_state_est[0].acc_x = Placeholder_measurement[1] * GRAVITATION;
				state_est_meas.imu_state_est[0].ts = Placeholder_timestamps[1];
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
				state_est_meas.baro_state_est[1].pressure = Placeholder_measurement[0];
				state_est_meas.baro_state_est[1].temperature = Placeholder_measurement[2];
				state_est_meas.baro_state_est[1].ts = Placeholder_timestamps[0];

				state_est_meas.imu_state_est[1].acc_x = Placeholder_measurement[1] * GRAVITATION;
				state_est_meas.imu_state_est[1].ts = Placeholder_timestamps[1];
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
				state_est_meas.baro_state_est[2].pressure = Placeholder_measurement[0];
				state_est_meas.baro_state_est[2].temperature = Placeholder_measurement[2];
				state_est_meas.baro_state_est[2].ts = Placeholder_timestamps[0];

				state_est_meas.imu_state_est[2].acc_x = Placeholder_measurement[1] * GRAVITATION;
				state_est_meas.imu_state_est[2].ts = Placeholder_timestamps[1];
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
		process_measurements(&ekf_state, &state_est_meas, &state_est_meas_prior, &environment);

		/* select noise models (dependent on detected flight phase and updated temperature in environment) */
		select_noise_models(&ekf_state, &flight_phase_detection, &environment);

		/* Start Kalman Update */

		/* Prediction Step */
		ekf_prediction(&ekf_state);

		/* update Step */
		if (ekf_state.num_z_active > 0) {
			select_ekf_observation_matrices(&ekf_state);
			ekf_update(&ekf_state);
		}
		else
		{
			memcpy(ekf_state.x_est, ekf_state.x_priori, sizeof(ekf_state.x_priori));
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
			update_state_est_data(&state_est_data, &ekf_state);
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

    if (STATE_ESTIMATION_FREQUENCY == 1000) {
        float A_init[NUMBER_STATES][NUMBER_STATES] = {{1.0E-0, 1.0E-3, 5.0E-7}, {0, 1.0E-0, 1.0E-3}, {0.0, 0.0, 1.0E-0}};
        float B_init[NUMBER_STATES][NUMBER_INPUTS] = {{5.0E-7}, {1.0E-3}, {0.0}};
        float G_init[NUMBER_STATES][NUMBER_NOISE] = {{5.0E-7}, {1.0E-3}, {0.0}};
        memcpy(ekf_state->Ad, A_init, sizeof(ekf_state->Ad));
        memcpy(ekf_state->Bd, B_init, sizeof(ekf_state->Bd));
        memcpy(ekf_state->Gd, G_init, sizeof(ekf_state->Gd));
    }
    else if (STATE_ESTIMATION_FREQUENCY == 200) {
        float A_init[NUMBER_STATES][NUMBER_STATES] = {{1.0E-0, 5.0E-3, 1.25E-5}, {0, 1.0E-0, 5.0E-3}, {0.0, 0.0, 1.0E-0}};
        float B_init[NUMBER_STATES][NUMBER_INPUTS] = {{1.25E-5}, {5.0E-3}, {0.0}};
        float G_init[NUMBER_STATES][NUMBER_NOISE] = {{1.25E-5}, {5.0E-3}, {0.0}};
        memcpy(ekf_state->Ad, A_init, sizeof(ekf_state->Ad));
        memcpy(ekf_state->Bd, B_init, sizeof(ekf_state->Bd));
        memcpy(ekf_state->Gd, G_init, sizeof(ekf_state->Gd));
    }
    else if (STATE_ESTIMATION_FREQUENCY == 100) {
        float A_init[NUMBER_STATES][NUMBER_STATES] = {{1.0E-0, 1.0E-2, 5.0E-5}, {0, 1.0E-0, 1.0E-2}, {0.0, 0.0, 1.0E-0}};
        float B_init[NUMBER_STATES][NUMBER_INPUTS] = {{5.0E-5}, {1.0E-2}, {0.0}};
        float G_init[NUMBER_STATES][NUMBER_NOISE] = {{5.0E-5}, {1.0E-2}, {0.0}};
        memcpy(ekf_state->Ad, A_init, sizeof(ekf_state->Ad));
        memcpy(ekf_state->Bd, B_init, sizeof(ekf_state->Bd));
        memcpy(ekf_state->Gd, G_init, sizeof(ekf_state->Gd));
    }
    else if (STATE_ESTIMATION_FREQUENCY == 1) {
        float A_init[NUMBER_STATES][NUMBER_STATES] = {{1.0E-0, 1.0E-0, 5.0E-1}, {0, 1.0E-0, 1.0E-0}, {0.0, 0.0, 1.0E-0}};
        float B_init[NUMBER_STATES][NUMBER_INPUTS] = {{5.0E-1}, {1.0E-0}, {0.0}};
        float G_init[NUMBER_STATES][NUMBER_NOISE] = {{5.0E-1}, {1.0E-0}, {0.0}};
        memcpy(ekf_state->Ad, A_init, sizeof(ekf_state->Ad));
        memcpy(ekf_state->Bd, B_init, sizeof(ekf_state->Bd));
        memcpy(ekf_state->Gd, G_init, sizeof(ekf_state->Gd));
    }
    else {
        memset(ekf_state->Ad, 0, sizeof(ekf_state->Ad));
        memset(ekf_state->Bd, 0, sizeof(ekf_state->Bd));
        memset(ekf_state->Gd, 0, sizeof(ekf_state->Gd));
    }

	float x_est_init[NUMBER_STATES] = {0, 0, 0};
	float P_est_init[NUMBER_STATES][NUMBER_STATES] = {{1.0E-9, 0, 0}, {0, 1.0E-12, 0}, {0, 0, 0}};

    memcpy(ekf_state->x_est, x_est_init, sizeof(x_est_init));
    memcpy(ekf_state->P_est, P_est_init, sizeof(P_est_init));

    memset(ekf_state->Q, 0, NUMBER_NOISE*NUMBER_NOISE*sizeof(ekf_state->Q[0][0]));
    memset(ekf_state->R, 0, NUMBER_MEASUREMENTS*NUMBER_MEASUREMENTS*sizeof(ekf_state->R[0][0]));

    memset(ekf_state->z, 0, NUMBER_MEASUREMENTS*sizeof(ekf_state->z[0]));
    memset(ekf_state->z_active, 0, NUMBER_MEASUREMENTS*sizeof(ekf_state->z_active[0]));
    ekf_state->num_z_active = 0;

    transpose(NUMBER_STATES, NUMBER_STATES, ekf_state->Ad, ekf_state->Ad_T);
    transpose(NUMBER_STATES, NUMBER_NOISE, ekf_state->Gd, ekf_state->Gd_T);
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

void select_ekf_observation_matrices(ekf_state_t *ekf_state){
    memset(ekf_state->H, 0, NUMBER_MEASUREMENTS*NUMBER_STATES*sizeof(ekf_state->H[0][0]));

    for(int i = 0; i < NUMBER_MEASUREMENTS; i++){
        if (ekf_state->z_active[i]) {
             /* activate contribution of measurement in measurement matrix */
            ekf_state->H[i][0] = 1;
        } else {
            /* set contributed measurement covariance to zero */
            ekf_state->R[i][i] = 0;
        }
    }

    transpose(NUMBER_MEASUREMENTS, NUMBER_STATES, ekf_state->H, ekf_state->H_T);
}

void ekf_update(ekf_state_t *ekf_state) {
    /* Update Step */
    /* y = z - H * x_priori */
    matvecprod(NUMBER_MEASUREMENTS, NUMBER_STATES, ekf_state->H, ekf_state->x_priori, ekf_state->y, true);
    vecsub(NUMBER_MEASUREMENTS, ekf_state->z, ekf_state->y, ekf_state->y);

    /* S = H * P_priori * H_T + R */
    matmul(NUMBER_MEASUREMENTS, NUMBER_STATES, NUMBER_STATES, ekf_state->H, ekf_state->P_priori, ekf_state->Placeholder_H_mult_P_priori, true);
    matmul(NUMBER_MEASUREMENTS, NUMBER_STATES, NUMBER_MEASUREMENTS, ekf_state->Placeholder_H_mult_P_priori, ekf_state->H_T, ekf_state->S, true);
    matadd(NUMBER_MEASUREMENTS,  NUMBER_MEASUREMENTS, ekf_state->S, ekf_state->R, ekf_state->S);

    /* Calculate Pseudoinverse of covariance innovation */
    memset(ekf_state->S_inv, 0, NUMBER_MEASUREMENTS*NUMBER_MEASUREMENTS*sizeof(ekf_state->S_inv[0][0]));
    inverse(NUMBER_MEASUREMENTS, ekf_state->S, ekf_state->S_inv, LAMBDA);

    /* K  = P_priori * H_T * S_inv */
    matmul(NUMBER_STATES, NUMBER_STATES, NUMBER_MEASUREMENTS, ekf_state->P_priori, ekf_state->H_T, ekf_state->Placeholder_P_priori_mult_H_T, true);
    matmul(NUMBER_STATES, NUMBER_MEASUREMENTS, NUMBER_MEASUREMENTS, ekf_state->Placeholder_P_priori_mult_H_T, ekf_state->S_inv, ekf_state->K, true);

    /* x_est = x_priori + K*y */
    matvecprod(NUMBER_STATES, NUMBER_MEASUREMENTS, ekf_state->K, ekf_state->y, ekf_state->x_est, true);
    vecadd(NUMBER_STATES, ekf_state->x_priori, ekf_state->x_est, ekf_state->x_est);


    /* P_est = (eye(NUMBER_STATES) - K*H)*P_priori */
    eye(NUMBER_STATES, ekf_state->Placeholder_eye);
    matmul(NUMBER_STATES, NUMBER_MEASUREMENTS, NUMBER_STATES, ekf_state->K, ekf_state->H, ekf_state->Placeholder_K_mult_H, true);
    matsub(NUMBER_STATES, NUMBER_STATES, ekf_state->Placeholder_eye, ekf_state->Placeholder_K_mult_H, ekf_state->Placeholder_P_est);
    matmul(NUMBER_STATES, NUMBER_STATES,  NUMBER_STATES, ekf_state->Placeholder_P_est, ekf_state->P_priori, ekf_state->P_est, true);
}

void update_state_est_data(state_est_data_t *state_est_data, ekf_state_t *ekf_state) {
    state_est_data->position_world[2] = (int32_t)(ekf_state->x_est[0] * 1000);
    state_est_data->velocity_rocket[0] = (int32_t)(ekf_state->x_est[1] * 1000);
    state_est_data->velocity_world[2] = (int32_t)(ekf_state->x_est[1] * 1000);
    state_est_data->acceleration_rocket[0] = (int32_t)(ekf_state->u[0] * 1000);
    state_est_data->acceleration_world[2] = (int32_t)(ekf_state->u[0] * 1000);
}

void process_measurements(ekf_state_t *ekf_state, state_est_meas_t *state_est_meas, state_est_meas_t *state_est_meas_prior, env *env) {
    float temp_meas[NUMBER_SENSORBOARDS];
    bool temp_meas_active[NUMBER_SENSORBOARDS];
    float acc_x_meas[NUMBER_SENSORBOARDS];
    bool acc_x_meas_active[NUMBER_SENSORBOARDS];

    for (int i = 0; i < NUMBER_SENSORBOARDS; i++){
        /* barometer */
        if (state_est_meas->baro_state_est[i].ts > state_est_meas_prior->baro_state_est[i].ts || state_est_meas->baro_state_est[i].ts == 0) {
            ekf_state->z[i] = state_est_meas->baro_state_est[i].pressure;
            ekf_state->z_active[i] = true;

            temp_meas[i] = state_est_meas->baro_state_est[i].temperature;
            temp_meas_active[i] = true;
        } else {
            ekf_state->z[i] = 0;
            ekf_state->z_active[i] = false;

            temp_meas[i] = 0;
            temp_meas_active[i] = false;
        }

        /* imu */
        if (state_est_meas->imu_state_est[i].ts > state_est_meas_prior->imu_state_est[i].ts || state_est_meas->imu_state_est[i].ts == 0) {
            acc_x_meas[i] = state_est_meas->imu_state_est[i].acc_x;
            acc_x_meas_active[i] = true;
        } else {
            acc_x_meas[i] = 0;
            acc_x_meas_active[i] = false;
        }
    }

    /* eliminate barometer measurements */
    /* TODO @maxi: Replace with sensor elimination by extrapolation */
    sensor_elimination_by_stdev(NUMBER_MEASUREMENTS, ekf_state->z, ekf_state->z_active);

    /* eliminate temperature measurements */
    sensor_elimination_by_stdev(NUMBER_MEASUREMENTS, temp_meas, temp_meas_active);

    /* eliminate accelerometer in rocket x-dir measurements */
    sensor_elimination_by_stdev(NUMBER_MEASUREMENTS, acc_x_meas, acc_x_meas_active);

    /* update num_z_active */
    ekf_state->num_z_active = 0;
    /* take the average of the active accelerometers in rocket-x dir as the state estimation input */
    ekf_state->u[0] = 0;
    int32_t num_acc_x_meas_active = 0;
    /* take the average of the temperature measurement  */
    float temp_meas_mean = 0;
    int32_t num_temp_meas_active = 0;
    for (int i = 0; i < NUMBER_MEASUREMENTS; i++){
        if (ekf_state->z_active[i]){
            ekf_state->num_z_active += 1;
        }
        if (acc_x_meas_active[i]) {
            ekf_state->u[0] += acc_x_meas[i];
            num_acc_x_meas_active += 1;
        }
        if (temp_meas[i]) {
            temp_meas_mean += temp_meas[i];
            num_temp_meas_active += 1;
        }
    }

    pressure2altitudeAGL(env, NUMBER_MEASUREMENTS, ekf_state->z, ekf_state->z_active, ekf_state->z);

    if (num_acc_x_meas_active > 0){
        ekf_state->u[0] /= num_acc_x_meas_active;
        /* gravity compensation for accelerometer */
        ekf_state->u[0] -= GRAVITATION;
    }
    if (num_temp_meas_active > 0){
        temp_meas_mean /= num_temp_meas_active;
        update_env(env, temp_meas_mean);
    }
}

void select_noise_models(ekf_state_t *ekf_state, flight_phase_detection_t *flight_phase_detection, env *env){
    float accelerometer_x_stdev;
    float barometer_stdev;

    // TODO @maxi: add different noise models for each mach regime
    switch (flight_phase_detection->flight_phase) {
        case AIRBRAKE_TEST:
        case RECOVERY:
        case IDLE:
            accelerometer_x_stdev = 0.0185409;
            barometer_stdev = 1.869;
        break;
        case THRUSTING:
            accelerometer_x_stdev = 1.250775;
            barometer_stdev = 13.000;
        break;
        case COASTING:
            accelerometer_x_stdev = 0.61803;
            barometer_stdev = 7.380;
        break;
        case DESCENT:
            accelerometer_x_stdev = 1.955133;
            barometer_stdev = 3.896;
        break;
    }

    for(int i = 0; i < NUMBER_NOISE; i++){
        ekf_state->Q[i][i] = pow(accelerometer_x_stdev, 2);
    }

    float p[1];
    float h[1] = {ekf_state->x_est[0]};
    bool h_active[1] = {true};
    altitudeAGL2pressure(env, 1, h, h_active, p);
    float h_grad = altitude_gradient(env, p[0]);
    float altitude_stdev = fabsf(barometer_stdev * h_grad);

    for(int i = 0; i < NUMBER_MEASUREMENTS; i++){
        ekf_state->R[i][i] = pow(altitude_stdev, 2);
    }
}

void sensor_elimination_by_stdev(int32_t n, float measurements[n], bool measurement_active[n]) {
    /* calculate mean of the sample */
    int32_t num_active = 0;
    float mean = 0;
    for (int i = 0; i < n; i++){
        if (measurement_active[i]) {
            num_active += 1;
            mean += measurements[i];
        }
    }
    if (num_active > 0){
        mean /= num_active;
    }

    /* calculate the standard deviation of the sample */
    float stdev = 0;
    for (int i = 0; i < n; ++i) {
        if (measurement_active[i]) {
            stdev += pow(measurements[i] - mean, 2);
        }
    }
    if (num_active > 0){
        stdev = sqrt(stdev / num_active);
    }

    /* deactivate measurements if they are too far off the mean */
    for (int i = 0; i < n; ++i) {
        if (measurement_active[i]) {
            if (fabsf(measurements[i] - mean) > 2.0 * stdev) {
                measurement_active[i] = false;
            }
        }
    }
}
