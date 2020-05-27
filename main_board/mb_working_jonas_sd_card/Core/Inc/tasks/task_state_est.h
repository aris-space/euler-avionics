/*
 * task_state_est.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_STATE_EST_H_
#define INC_TASKS_TASK_STATE_EST_H_

/* Includes */
#include "cmsis_os.h"
#include "util.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "math_utils.h"
#include "Sensor_Fusion_Helper/env.h"
/* Constants */
#define STATE_ESTIMATION_FREQUENCY 1
/* Matrix Sizes */
#define NUMBER_STATES 3	/* NUMBER_STATES x NUMBER_STATES -> A Matrix */
#define NUMBER_INPUTS 1	/* NUMBER_STATES x NUMBER_INPUTS -> B Matrix */
#define NUMBER_NOISE 1	/* NUMBER_STATES x NUMBER_NOISE -> G Matrix */
#define NUMBER_SENSOR 6 /* NUMBER_SENSOR x NUMBER_STATES -> H Matrix */
#define NUMBER_SENSORBOARDS 3 /* Number of Sensor Boards */
#define LAMBDA 0.1		/* Lambda for Moore Penrose Pseudoinverse */


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
	float H[NUMBER_SENSOR][NUMBER_STATES];
	float H_T[NUMBER_STATES][NUMBER_SENSOR];
	float Q[NUMBER_NOISE][NUMBER_NOISE];
	float R[NUMBER_SENSOR][NUMBER_SENSOR];

	/* State Variables */
    float u[NUMBER_INPUTS]; // inputs
	float x_est[NUMBER_STATES]; // estimated state (posteriori)
	float P_est[NUMBER_STATES][NUMBER_STATES]; // estimated covariance (posteriori)
	float x_priori[NUMBER_STATES]; // estimated state (priori)
	float P_priori[NUMBER_STATES][NUMBER_STATES]; // estimated covariance (priori)
    float z[NUMBER_SENSOR]; // measurements
	float y[NUMBER_SENSOR]; // state innovation
	float S[NUMBER_SENSOR][NUMBER_SENSOR]; // covariance innovation
	float S_inv[NUMBER_SENSOR][NUMBER_SENSOR];
	float K[NUMBER_STATES][NUMBER_SENSOR];

    /* Placeholder Variables for increased speed */
	float Placeholder_Ad_mult_P_est[NUMBER_STATES][NUMBER_STATES];
    float Placeholder_Gd_mult_Q[NUMBER_STATES][NUMBER_NOISE];
	float Placeholder_H_mult_P_priori[NUMBER_SENSOR][NUMBER_SENSOR];
	float Placeholder_P_priori_mult_H_T[NUMBER_STATES][NUMBER_SENSOR];
	float Placeholder_P_est[NUMBER_STATES][NUMBER_STATES];
    float Placeholder_eye[NUMBER_STATES][NUMBER_STATES];
    float Placeholder_K_mult_H[NUMBER_STATES][NUMBER_STATES];
} ekf_state_t;

/* State Estimation Data struct */
typedef struct {
	baro_data_t baro_data[NUMBER_SENSORBOARDS];
	imu_data_t imu_data[NUMBER_SENSORBOARDS];
} state_est_meas_t;


/* Tasks */
void vTaskStateEst(void *argument);

#endif /* INC_TASKS_TASK_STATE_EST_H_ */
