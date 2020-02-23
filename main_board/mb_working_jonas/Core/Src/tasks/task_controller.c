/*
 * task_controller.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "tasks/task_controller.h"


void CalcPolynomial(int32_t ref_height, int32_t *ref_vel, int32_t gains[], int32_t coefficients[][POLY_DEG]);


void vTaskController(void *argument) {
	/* For periodic update */
	uint32_t tick_count, tick_update;

	/* Polynomial Coefficients for Gains and Reference Traj */
	int32_t coeff[4][POLY_DEG] = { 0 };

	/* State Estimation Values */
	int32_t sf_velocity = 0;
	int32_t sf_height = 0;

	/* Gain Values and Trajectory Values to increase speed */
	int32_t gains[3] = { 0 };
	int32_t ref_vel = 0;
	int32_t vel_error = 0;
	int32_t control_input = 0;

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / CONTROLLER_SAMPLING_FREQ;
	while (1) {
		/* Tick Update */
		tick_count += tick_update;

		/* Update Sensor Fusion Variables */
		/* TODO once Mutex Managment has been set up */

		/* caluclate Gains and Reference velocity for given height */
		CalcPolynomial(sf_height, &ref_vel, gains, coeff);

		/* Calculate Velocity Error */
		vel_error = sf_velocity - ref_vel;

		/* Calculate Control Input */
		/* TODO */

		/* Anti Windup */
		/* TODO */

		/* Send Control Output to motor control Task */
		/* TODO */

		/* Sleep */
		osDelayUntil(tick_count);
	}
}


/* Does the Polynomial Calculation of the reference velocity */
void CalcPolynomial(int32_t ref_height, int32_t *ref_vel, int32_t gains[], int32_t coefficients[][POLY_DEG]){
	/* For Speed */
	uint32_t x_placeholder = 0;

	/* For loop */
	for(int i = 0; i < POLY_DEG; ++i){
		x_placeholder = pow(ref_height, (POLY_DEG - 1 - i));
		gains[0] += coefficients[0][i] * x_placeholder;
		gains[1] += coefficients[1][i] * x_placeholder;
		gains[2] += coefficients[2][i] * x_placeholder;
		*ref_vel += coefficients[3][i] * x_placeholder;
	}

}
