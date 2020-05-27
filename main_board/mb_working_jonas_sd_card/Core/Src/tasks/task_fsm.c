/*
 * task_fsm.c
 *
 *  Created on: May 24, 2020
 *      Author: Jonas
 */

#include "tasks/task_fsm.h"

void detect_flight_phase(flight_phase_detection_t *flight_phase_detection, state_est_data_t *state_est_data, env *env);

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

	/* environment data */
	env environment;
	env dummy_env;
	init_env(&dummy_env);
	init_env(&environment);


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
		/* Update Environment */
		if(osMutexGetOwner(environment_mutex) == NULL){
			dummy_env = global_env;
			if(osMutexGetOwner(state_est_mutex) == NULL){
				environment = dummy_env;
			}
		}

		/* get Flight Phase update */
		detect_flight_phase(&flight_phase_detection, &state_est_data_fsm, &environment);

		/* TODO Write NEW State in GLobal Variable */
		if(osMutexAcquire(fsm_mutex, 10) == osOK){
			global_flight_phase_detection = flight_phase_detection;
			osMutexRelease(fsm_mutex);
		}

		/* Sleep */
		osDelayUntil(tick_count);
	}
}


void detect_flight_phase(flight_phase_detection_t *flight_phase_detection, state_est_data_t *state_est_data, env *environment)

{
    /* determine state transition events */
    switch (flight_phase_detection->flight_phase) {
        case IDLE:
            if (((float)(state_est_data->acceleration_rocket[0]))/1000 > 20) {
                flight_phase_detection->num_samples_positive += 1;
                if (flight_phase_detection->num_samples_positive >= 4) {
                    flight_phase_detection->flight_phase = THRUSTING;
                    flight_phase_detection->num_samples_positive = 0;
                }
            }
        break;

        case THRUSTING:
            if (((float)(state_est_data->acceleration_rocket[0]))/1000 < 0) {
                flight_phase_detection->num_samples_positive += 1;
                if (flight_phase_detection->num_samples_positive >= 4) {
                    flight_phase_detection->flight_phase = COASTING;
                    flight_phase_detection->num_samples_positive = 0;
                }
            }
        break;

        case COASTING:
            if (((float)(state_est_data->velocity_world[2]))/1000 < 0) {
                flight_phase_detection->num_samples_positive += 1;
                if (flight_phase_detection->num_samples_positive >= 4) {
                    flight_phase_detection->flight_phase = DESCENT;
                    flight_phase_detection->num_samples_positive = 0;
                }
            }
        break;

        case DESCENT:
            if (((float)(state_est_data->position_world[2]))/1000 < 20) {
                flight_phase_detection->num_samples_positive += 1;
                if (flight_phase_detection->num_samples_positive >= 4) {
                    flight_phase_detection->flight_phase = RECOVERY;
                    flight_phase_detection->num_samples_positive = 0;
                }
            }
        break;

        default:
        break;
    }

    flight_phase_detection->mach_number = mach_number(environment, ((float)state_est_data->velocity_rocket[0])/1000);

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
