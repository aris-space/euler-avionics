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

  /* average Gyro */
//  float average_gyro[3] = {0};
//  float sum_gyro[3] = {0};

  /* average Accel */
//  float average_accel[3] = {0};
//  float sum_accel[3] = {0};

  uint16_t calibrate_count = 0;
//  uint16_t calibrate_count_imu = 0;

  /* Airbrake extension */
  uint32_t airbrake_ext_meas = 0;


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

    /* calculate averaging */
    // TODO [luca] this can be dangerous if one of our baros is faulty, we also dont see that in our telemetry
    if (state_est_state.flight_phase_detection.flight_phase == IDLE) {
      sum_press +=
          state_est_state.state_est_meas.baro_data[0].pressure +
		  state_est_state.state_est_meas.baro_data[1].pressure +
		  state_est_state.state_est_meas.baro_data[2].pressure;
      sum_temp += (state_est_state.state_est_meas.baro_data[0].temperature +
    		  state_est_state.state_est_meas.baro_data[1].temperature +
			  state_est_state.state_est_meas.baro_data[2].temperature) /
                  100;
//      for(int i = 0; i < 6; i++){
//    	  sum_gyro[0] += state_est_state.state_est_meas.imu_data[i].gyro_x;
//    	  sum_gyro[1] += state_est_state.state_est_meas.imu_data[i].gyro_y;
//    	  sum_gyro[2] += state_est_state.state_est_meas.imu_data[i].gyro_z;
//    	  sum_accel[0] += state_est_state.state_est_meas.imu_data[i].acc_x;
//    	  sum_accel[1] += state_est_state.state_est_meas.imu_data[i].acc_y;
//    	  sum_accel[2] += state_est_state.state_est_meas.imu_data[i].acc_z;
//      }
//      calibrate_count_imu += 6;
      calibrate_count += 3;
      if (calibrate_count > 150) {
        average_press = sum_press / (float)calibrate_count;
        average_temp = sum_temp / (float)calibrate_count;
//        for(int i = 0; i< 3; i++){
//        	average_accel[i] = sum_accel[i] / (float)calibrate_count_imu;
//        	sum_accel[i] = 0;
//        	average_gyro[i] = sum_gyro[i] / (float)calibrate_count_imu;
//        	sum_gyro[i] = 0;
//        }


        sum_press = 0;
        sum_temp = 0;
        calibrate_count = 0;
//        calibrate_count_imu = 0;
      }
    }

    /* Remove Bias from all IMU Data */
//    for(int i = 0; i < 6; i++){
//    	state_est_state.state_est_meas.imu_data[i].acc_x -= average_accel[0];
//    	state_est_state.state_est_meas.imu_data[i].acc_y -= average_accel[1];
//    	state_est_state.state_est_meas.imu_data[i].acc_z -= (average_accel[2] + 9.81);
//    	state_est_state.state_est_meas.imu_data[i].gyro_x -= average_gyro[0];
//    	state_est_state.state_est_meas.imu_data[i].gyro_y -= average_gyro[1];
//    	state_est_state.state_est_meas.imu_data[i].gyro_z -= average_gyro[2];
//    }


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

//    usb_print("[DBG] Height: %d; Velocity: %d; Airbrake_ext: %d; t: %lu\n",
//    				state_est_data_global.position_world[2],
//    				state_est_data_global.velocity_world[2],
//					(int32_t)(state_est_state.state_est_meas.airbrake_extension*1000),
//     tick_count);

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
    /* TODO: Check if the state estimation can do this for the given frequency
     */

    /* Sleep */
    osDelayUntil(tick_count);
  }
}
