/*
 * task_fsm.c
 *
 *  Created on: May 24, 2020
 *      Author: Jonas
 */

#include "tasks/task_fsm.h"

void detect_flight_phase(flight_phase_detection_t *flight_phase_detection, state_est_data_t *state_est_data);

void reset_flight_phase_detection(flight_phase_detection_t *flight_phase_detection);


void vTaskFSM(void *argument) {
	/* For periodic update */
	uint32_t tick_count, tick_update;

	/* Phase detection struct */
	flight_phase_detection_t flight_phase_detection = { 0 };
	reset_flight_phase_detection(&flight_phase_detection);

	/*State Estimation data */
	state_est_data_t state_est_data_fsm = { 0 };
	state_est_data_t state_est_data_fsm_dummy = { 0 };


	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / FSM_SAMPLING_FREQ;

	while (1) {
		/* Tick Update */
		tick_count += tick_update;

		/* update state estimation data */
		if(osMutexGetOwner(state_est_mutex) == NULL){
			/* TODO: Check correct indexing */
			/* the value is multiplied by 1000 for conversion to int datatype for easy transport
			 * careful in other tasks!
			 */
			state_est_data_fsm_dummy = state_est_data;
			if(osMutexGetOwner(state_est_mutex) == NULL){
				state_est_data_fsm = state_est_data_fsm_dummy;
			}
		}

		/* get Flight Phase update */
		detect_flight_phase(&flight_phase_detection, &state_est_data_fsm);

		/* TODO Write NEW State in GLobal Variable */

		/* Sleep */
		osDelayUntil(tick_count);
	}
}


void detect_flight_phase(flight_phase_detection_t *flight_phase_detection, state_est_data_t *state_est_data) {
    /* altitude above ground level in world frame [m]*/
    float X_z = (float)(state_est_data->altitude_above_GL);
    X_z = X_z/1000;
    /* vertical velocity in world frame [m/s]*/
    float V_z = (float)(state_est_data->velocity);
    V_z = V_z/1000;
    /* acceleration in x-dir in rocket frame [m/s^2]*/
    float Vdot_z = (float)(state_est_data->acceleration);
    Vdot_z = Vdot_z/1000;

    /* determine state transition events */
    switch (flight_phase_detection->flight_phase) {
        case IDLE:
            if (Vdot_z > 20) {
                flight_phase_detection->num_samples_positive += 1;
                if (flight_phase_detection->num_samples_positive >= 4) {
                    flight_phase_detection->flight_phase = THRUSTING;
                    flight_phase_detection->num_samples_positive = 0;
                }
            }
        break;

        case THRUSTING:
            if (Vdot_z < 0) {
                flight_phase_detection->num_samples_positive += 1;
                if (flight_phase_detection->num_samples_positive >= 4) {
                    flight_phase_detection->flight_phase = COASTING;
                    flight_phase_detection->num_samples_positive = 0;
                }
            }
        break;

        case COASTING:
            if (V_z < 0) {
                flight_phase_detection->num_samples_positive += 1;
                if (flight_phase_detection->num_samples_positive >= 4) {
                    flight_phase_detection->flight_phase = DESCENT;
                    flight_phase_detection->num_samples_positive = 0;
                }
            }
        break;

        case DESCENT:
            if (X_z < 20) {
                flight_phase_detection->num_samples_positive += 1;
                if (flight_phase_detection->num_samples_positive >= 4) {
                    flight_phase_detection->flight_phase = RECOVERY;
                    flight_phase_detection->num_samples_positive = 0;
                }
            }
        break;

        case RECOVERY:
        break;
    }

    // TODO @maxi: Implement update of mach number
    flight_phase_detection->mach_number = 0.0;

    /* determine the mach regime */
    if (flight_phase_detection->mach_number >= 1.3) {
        flight_phase_detection->mach_regime = SUPERSONIC;
    } else if (flight_phase_detection->mach_number >= 0.8)
    {
        flight_phase_detection->mach_regime = TRANSONIC;
    } else
    {
        flight_phase_detection->mach_regime = SUBSONIC;
    }
}


void reset_flight_phase_detection(flight_phase_detection_t *flight_phase_detection){
    flight_phase_detection->flight_phase = IDLE;
    flight_phase_detection->mach_regime = SUBSONIC;
    flight_phase_detection->mach_number = 0.0;
    flight_phase_detection->num_samples_positive = 0;
}
