/*
 * task_controller.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "tasks/task_controller.h"


void CalcPolynomial(float ref_height, float *ref_vel, float gains[], float coefficients[][POLY_DEG]);


void vTaskController(void *argument) {
	/* For periodic update */
	uint32_t tick_count, tick_update;

	/* Polynomial Coefficients for Gains and Reference Traj */
	float coeff[4][POLY_DEG] = { 0 };

	/* State Estimation Values */
	float sf_velocity = 0;
	float sf_height = 0;

	/* Gain Values and Trajectory Values to increase speed */
	float gains[3] = { 0 };
	float ref_vel = 0;
	float vel_error = 0;
	float control_input = 0;
	float integrated_error = 0;
	float previous_control_input = 0;
	uint32_t delta_t = 1 / CONTROLLER_SAMPLING_FREQ; 	/* That is probably very optimistic! */

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
		control_input = - gains[0] * vel_error - gains[1] * integrated_error
				- gains[2] * (previous_control_input - OPT_TRAJ_CONTROL_INPUT)
				+ previous_control_input;

		control_input = fmax(0, fmin(control_input, 1));

		/* Send Control Output to motor control Task */
		/* TODO */

		/* Update Integrated Error */
		integrated_error = fmax(-10, fmin(integrated_error + delta_t*vel_error, 10));

		/* Update Previous Control Input */
		previous_control_input = control_input;
		/* Sleep */
		osDelayUntil(tick_count);
	}
}


/* Does the Polynomial Calculation of the reference velocity */
void CalcPolynomial(float ref_height, float *ref_vel, float gains[], float coefficients[][POLY_DEG]){
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
