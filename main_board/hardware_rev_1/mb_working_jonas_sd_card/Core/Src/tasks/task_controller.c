/*
 * task_controller.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "tasks/task_controller.h"

/* Abbreviation 'aw' is used to described everything related to the antiwindup */


void vTaskController(void *argument) {
	/* For periodic update */
	uint32_t tick_count, tick_update;

	state_est_data_t state_placeholder;
    flight_phase_detection_t flight_phase_detection_placeholder = { 0 };
    flight_phase_detection_t current_flight_phase_detection = { 0 };

    /* Initialize the control_data struct */
    control_data_t control_data = { 0 };
    control_data_init(&control_data);

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / CONTROLLER_SAMPLING_FREQ;

	while (1) {
		/* Tick Update */
		tick_count += tick_update;

		/* Update Sensor Fusion Variables */
		if (osMutexGetOwner(state_est_mutex) == NULL) {
			state_placeholder.position_world[2] = state_est_data_global.position_world[2];
			state_placeholder.velocity_world[2] = state_est_data_global.velocity_world[2];
			if (osMutexGetOwner(state_est_mutex) == NULL) {
				control_data.sf_velocity = ((float)state_placeholder.position_world[2]) / 1000;
				control_data.sf_ref_altitude_AGL = ((float)state_placeholder.velocity_world[2]) / 1000;
			}
		}

		/* update flight Phase */
		if (osMutexGetOwner(fsm_mutex) == NULL) {
			flight_phase_detection_placeholder.flight_phase = global_flight_phase_detection.flight_phase;
			flight_phase_detection_placeholder.mach_regime = global_flight_phase_detection.mach_regime;
			flight_phase_detection_placeholder.mach_number = global_flight_phase_detection.mach_number;
			if (osMutexGetOwner(fsm_mutex) == NULL) {
				current_flight_phase_detection.flight_phase = flight_phase_detection_placeholder.flight_phase;
				current_flight_phase_detection.mach_regime = flight_phase_detection_placeholder.mach_regime;
				current_flight_phase_detection.mach_number = flight_phase_detection_placeholder.mach_number;
			}
		}

		/** MAKE SURE THE RIGHT CONTROLLER IS ACTIVE IS ACTIVE!!!!! **/
        if(LQR_ACTIVE) {
            compute_control_input(&control_data, &current_flight_phase_detection);
        }
        else {
            compute_test_control_input(&control_data);
        }
		/* Write Control Input into Global Variable */
		if (osMutexAcquire(controller_mutex, 10) == osOK) {
			controller_output_global = (int32_t)(control_data.control_input * 1000);
			osMutexRelease(controller_mutex);
		}

		/* Log to SD Card */

		logControllerOutput(osKernelGetTickCount(), controller_output_global);


		/* Sleep */
		osDelayUntil(tick_count);
	}
}
