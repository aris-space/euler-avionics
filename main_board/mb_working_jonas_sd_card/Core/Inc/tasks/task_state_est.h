/*
 * task_state_est.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_STATE_EST_H_
#define INC_TASKS_TASK_STATE_EST_H_

/* Includes */
#include "Helper_Functions/env.h"
#include "tasks/task_fsm.h"
#include "cmsis_os.h"
#include "util.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "math_utils.h"
/* Constants */
#define STATE_ESTIMATION_FREQUENCY 1
/* Matrix Sizes */
#define NUMBER_STATES 3	/* NUMBER_STATES x NUMBER_STATES -> A Matrix */
#define NUMBER_INPUTS 1	/* NUMBER_STATES x NUMBER_INPUTS -> B Matrix */
#define NUMBER_NOISE 1	/* NUMBER_STATES x NUMBER_NOISE -> G Matrix */
#define NUMBER_MEASUREMENTS 6 /* NUMBER_SENSOR x NUMBER_STATES -> H Matrix */
#define NUMBER_SENSORBOARDS 3 /* Number of Sensor Boards */
#define MAX_LENGTH_ROLLING_MEMORY 18
#define LAMBDA 0.0001		/* Lambda for Moore Penrose Pseudoinverse */
#define PI 3.1415


/* Commands */

/* Extern */
/* Sensor Board 1 */
extern osMutexId_t sb1_mutex;
extern sb_data_t sb1_data;

/* Sensor Board 2 */
extern osMutexId_t sb2_mutex;
extern sb_data_t sb2_data;

/* Sensor Board 3 */
extern osMutexId_t sb3_mutex;
extern sb_data_t sb3_data;

/* State Estimation Mutex */
extern osMutexId_t state_est_mutex;
extern state_est_data_t state_est_data;

/* fsm Mutex */
extern osMutexId_t fsm_mutex;
extern osMutexId_t environment_mutex;
extern flight_phase_detection_t global_flight_phase_detection;
extern env global_env;


/* State Estimation Full State */
typedef struct {
	/* Fixed Variables */
    float Ad[NUMBER_STATES][NUMBER_STATES];
	float Ad_T[NUMBER_STATES][NUMBER_STATES];
	float Bd[NUMBER_STATES][NUMBER_INPUTS];
	float Gd[NUMBER_STATES][NUMBER_NOISE];
	float Gd_T[NUMBER_NOISE][NUMBER_STATES];
	float H[NUMBER_MEASUREMENTS][NUMBER_STATES];
	float H_T[NUMBER_STATES][NUMBER_MEASUREMENTS];
	float Q[NUMBER_NOISE][NUMBER_NOISE];
	float R[NUMBER_MEASUREMENTS][NUMBER_MEASUREMENTS];

	/* State Variables */
    float u[NUMBER_INPUTS]; // inputs
	float x_est[NUMBER_STATES]; // estimated state (posteriori)
	float P_est[NUMBER_STATES][NUMBER_STATES]; // estimated covariance (posteriori)
	float x_priori[NUMBER_STATES]; // estimated state (priori)
	float P_priori[NUMBER_STATES][NUMBER_STATES]; // estimated covariance (priori)
    float z[NUMBER_MEASUREMENTS]; // measurements
	float y[NUMBER_MEASUREMENTS]; // state innovation
	float S[NUMBER_MEASUREMENTS][NUMBER_MEASUREMENTS]; // covariance innovation
	float S_inv[NUMBER_MEASUREMENTS][NUMBER_MEASUREMENTS];
	float K[NUMBER_STATES][NUMBER_MEASUREMENTS];

	/* Specifies which measurements should be included in the Kalman Update */
	bool z_active[NUMBER_MEASUREMENTS];
	uint8_t num_z_active;

    /* Placeholder Variables for increased speed */
	float Placeholder_Ad_mult_P_est[NUMBER_STATES][NUMBER_STATES];
    float Placeholder_Gd_mult_Q[NUMBER_STATES][NUMBER_NOISE];
	float Placeholder_H_mult_P_priori[NUMBER_MEASUREMENTS][NUMBER_MEASUREMENTS];
	float Placeholder_P_priori_mult_H_T[NUMBER_STATES][NUMBER_MEASUREMENTS];
	float Placeholder_P_est[NUMBER_STATES][NUMBER_STATES];
    float Placeholder_eye[NUMBER_STATES][NUMBER_STATES];
    float Placeholder_K_mult_H[NUMBER_STATES][NUMBER_STATES];
} ekf_state_t;

/* State Estimation Baro struct */
typedef struct {
	float pressure;
	float temperature;
	timestamp_t ts;
}baro_state_est_t;


/* State Estimation IMU struct */
typedef struct {
	float gyro_x, gyro_y, gyro_z;
	float acc_x, acc_y, acc_z;
	timestamp_t ts;
}imu_state_est_t;

/* State Estimation combined Data struct */
typedef struct {
    /* pressure in [Pa] and temperature in [°C] */
	baro_state_est_t baro_state_est[NUMBER_SENSORBOARDS];
    /* acceleration in [m/s^2] and angular velocity in [rad/s] */
    /* all in rocket frame where x-dir is along length of rocket */
	imu_state_est_t imu_state_est[NUMBER_SENSORBOARDS];
} state_est_meas_t;

typedef struct {
    int32_t memory_length;
    timestamp_t timestamps[MAX_LENGTH_ROLLING_MEMORY];
    float measurements[MAX_LENGTH_ROLLING_MEMORY];
    float noise_stdev;
    double polyfit_coeffs;
} extrapolation_rolling_memory_t;


/* Tasks */
void vTaskStateEst(void *argument);

#endif /* INC_TASKS_TASK_STATE_EST_H_ */
