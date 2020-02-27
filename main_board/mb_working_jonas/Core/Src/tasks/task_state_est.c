/*
 * task_state_est.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "tasks/task_state_est.h"

void Initialise_Kalman(float Ad[A_SIZE][A_SIZE],
		float Bd[A_SIZE][B_SIZE], float Gd[A_SIZE][G_SIZE],
		float H[NUMBER_SENSOR][A_SIZE], float Q[A_SIZE][A_SIZE],
		float R[NUMBER_SENSOR][NUMBER_SENSOR]);

void M_Calc_Size_A_vec(float A[A_SIZE][A_SIZE], float x[A_SIZE], float y[A_SIZE]);

void vTaskStateEst(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;

	/* Variables */
	float Ad[A_SIZE][A_SIZE] = { 0 };
	float Ad_T[A_SIZE][A_SIZE] = { 0 };
	float Bd[A_SIZE][B_SIZE] = { 0 };
	float Gd[A_SIZE][G_SIZE] = { 0 };
	float Gd_T[G_SIZE][A_SIZE] = { 0 };
	float H[NUMBER_SENSOR][A_SIZE] = { 0 };
	float H_T[A_SIZE][NUMBER_SENSOR] = { 0 };
	float Q[A_SIZE][A_SIZE] = { 0 };
	float R[NUMBER_SENSOR][NUMBER_SENSOR] = { 0 };
	float x_priori[A_SIZE] = { 0 };
	float x_est[A_SIZE] = { 0 };
	float P_priori[A_SIZE][A_SIZE] = { 0 };
	float Placeholder[A_SIZE][A_SIZE] = { 0 };
	float P_est[A_SIZE][A_SIZE] = { 0 };

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / STATE_ESTIMATION_FREQUENCY;
	for (;;) {
		tick_count += tick_update;
		/* Prediction Step */
		/* Calculation of x_priori */
		for(int j = 0; j < A_SIZE; j++){
			for(int i = 0; i < A_SIZE; i++){
				x_priori[j] += Ad[j][i] * x_est[i];
			}
			for(int k = 0; k < B_SIZE; k++){
				x_priori[j] += Bd[j][k] * u[k];
			}
		}

		/* Calculation of P_priori */
		for(int j = 0; j < A_SIZE; j++){
			for(int i = 0; i < A_SIZE; i++){
				for(int k = 0; k < A_SIZE; k++){
					Placeholder[j][i] +=  Ad[j][k] * P_est[k][i];
				}
			}
		}

		osDelayUntil(tick_count);
	}
}


void Initialise_Kalman(float Ad[A_SIZE][A_SIZE],
		float Bd[A_SIZE][B_SIZE], float Gd[A_SIZE][G_SIZE],
		float H[NUMBER_SENSOR][A_SIZE], float Q[A_SIZE][A_SIZE],
		float R[NUMBER_SENSOR][NUMBER_SENSOR]){

	float A_init[A_SIZE][A_SIZE] = {{0, 1, 0}, {0, 0, 1}, {0, 0, 0}};
	float B_init[A_SIZE][B_SIZE] = {{0}, {1}, {0}};
	float G_init[A_SIZE][G_SIZE] = {{0}, {1}, {0}};
	float H_init[NUMBER_SENSOR][A_SIZE] = {{1, 0, 0},{1, 0, 0}, {1, 0, 0}};
	float Q_init[A_SIZE][A_SIZE] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
	float R_init[NUMBER_SENSOR][NUMBER_SENSOR] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};


	H = H_init;
	Q = Q_init;
	R = R_init;
	Ad = A_init;
	Bd = B_init;
	Gd = G_init;


}

void M_Calc_Size_A_vec(float A[A_SIZE][A_SIZE], float x[A_SIZE], float y[A_SIZE]){
	for(int j = 0; j < A_SIZE; j++){
		for(int i = 0; i < A_SIZE; i++){
			y[j] += A[j][i] * x[i];
		}
	}
}
