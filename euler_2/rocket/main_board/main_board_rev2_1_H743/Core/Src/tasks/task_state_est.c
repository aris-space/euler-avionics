/*
 * task_state_est.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "tasks/task_state_est.h"
#include "../../aris-euler-state-estimation/Inc/state_est.h"
#include "util/logging_util.h"
#include "util/util.h"
#include "util/mutex.h"

void vTaskStateEst(void *argument) {
  /* For periodic update */
  uint32_t tick_count, tick_update;

  /* Initialise Variables */
  state_est_state_t state_est_state = { 0 };
  reset_state_est_state(PRESSURE_REFERENCE, TEMPERATURE_REFERENCE, &state_est_state);

  command_e telemetry_command = IDLE_COMMAND;


  /* average Temperature */
  float average_temp = 0;
  float sum_temp = 0;

  /* average Pressure */
  float average_press = 0;
  float sum_press = 0;

  uint16_t calibrate_count = 0;
  uint8_t chosen_baro = 0;

  /* Airbrake extension */
  int32_t airbrake_ext_meas = 0;


  /* reset counter */
  uint32_t reset_counter = 0;
  bool was_reset = false;

  osDelay(900);

  /* Infinite loop */
  tick_count = osKernelGetTickCount();
  tick_update = osKernelGetTickFreq() / STATE_ESTIMATION_FREQUENCY;

  while (1) {
    tick_count += tick_update;

    /* Acquire New Command */
    read_mutex(&command_mutex, &global_telemetry_command, &telemetry_command,
              sizeof(global_telemetry_command));

    /*
     * Check if we need to reset the state estimation
     * and if we are in idle state to be able
     * to do so
     */
    if (state_est_state.flight_phase_detection.flight_phase == IDLE &&
        global_telemetry_command == CALIBRATE_SENSORS) {
    	reset_state_est_state(average_press, average_temp, &state_est_state);
    }

    /* Reset the whole thing automatically after 30 Seconds of running */
    if (reset_counter > 5 * STATE_ESTIMATION_FREQUENCY && !was_reset) {
    	reset_state_est_state(average_press, average_temp, &state_est_state);
      was_reset = true;
    }
    reset_counter++;

    /* Acquire the Sensor data */

    /* Sensor Board 1 */
    read_mutex_state_est(&sb1_mutex, &state_est_state.state_est_meas, &sb1_global, 1);

    /* Sensor Board 2 */
    read_mutex_state_est(&sb2_mutex, &state_est_state.state_est_meas, &sb2_global, 2);

    /* Sensor Board 3 */
    read_mutex_state_est(&sb3_mutex, &state_est_state.state_est_meas, &sb3_global, 3);

    /* Check for Working Baro */
    if((state_est_state.state_est_meas.baro_data[2].pressure < BARO_UPPER_BOUND) &&
    		(state_est_state.state_est_meas.baro_data[2].pressure > BARO_LOWER_BOUND) &&
			(state_est_state.state_est_meas.baro_data[2].temperature < TEMP_UPPER_BOUND) &&
			(state_est_state.state_est_meas.baro_data[2].temperature > TEMP_LOWER_BOUND)){
    	chosen_baro = 2;
    }
    else if((state_est_state.state_est_meas.baro_data[1].pressure < BARO_UPPER_BOUND) &&
    		(state_est_state.state_est_meas.baro_data[1].pressure > BARO_LOWER_BOUND) &&
			(state_est_state.state_est_meas.baro_data[1].temperature < TEMP_UPPER_BOUND) &&
			(state_est_state.state_est_meas.baro_data[1].temperature > TEMP_LOWER_BOUND)){
    	chosen_baro = 1;
    }
    else{
    	chosen_baro = 0;
    }

    /* calculate averaging */
    if (state_est_state.flight_phase_detection.flight_phase == IDLE) {
      sum_press += state_est_state.state_est_meas.baro_data[chosen_baro].pressure;
      sum_temp += state_est_state.state_est_meas.baro_data[chosen_baro].temperature;

      calibrate_count += 1;
      if (calibrate_count > 150) {
        average_press = sum_press / (float)calibrate_count;
        average_temp = sum_temp / (float)calibrate_count;

        sum_press = 0;
        sum_temp = 0;
        calibrate_count = 0;

      }
    }


    /* get current airbrake extension */
    read_mutex(&airbrake_ext_mutex, &global_airbrake_ext_meas,
        		   &airbrake_ext_meas, sizeof(global_airbrake_ext_meas));

    /* write into state_est_state */
    state_est_state.state_est_meas.airbrake_extension = ((float)airbrake_ext_meas)/1000;

    /* get new Phase Detection*/
    read_mutex(&fsm_mutex, &global_flight_phase_detection,
    		       &state_est_state.flight_phase_detection, sizeof(state_est_state.flight_phase_detection));

    /* Kalman Filter Step */
    state_est_step(tick_count, &state_est_state, false);

    /* Update global State Estimation Data */
    if (acquire_mutex(&state_est_mutex) == osOK) {
    	state_est_data_global = state_est_state.state_est_data;
      release_mutex(&state_est_mutex);
    }


    /* Update env for FSM */
    if (acquire_mutex(&fsm_mutex) == osOK) {
      global_env = state_est_state.env;
      release_mutex(&fsm_mutex);
    }

    /* Write to logging system */
#if STATE_ESTIMATION_TYPE == 1
    log_estimator_var(osKernelGetTickCount(), state_est_data_global, ESTIMATOR_VAR_1D);
#elif STATE_ESTIMATION_TYPE == 2
    log_estimator_var(osKernelGetTickCount(), state_est_data_global, ESTIMATOR_VAR_3D);
#endif

    /* Sleep */
    osDelayUntil(tick_count);
  }
}
