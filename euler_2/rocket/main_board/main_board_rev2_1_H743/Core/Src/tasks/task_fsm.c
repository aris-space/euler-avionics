/*
 * task_fsm.c
 *
 *  Created on: May 24, 2020
 *      Author: Jonas
 */

#include "../aris-euler-state-estimation/Inc/flight_phase_detection.h"
#include "util/logging_util.h"
#include "tasks/task_fsm.h"

void vTaskFsm(void *argument) {
  /* For periodic update */
  uint32_t tick_count, tick_update;

  /* Phase detection struct */
  flight_phase_detection_t flight_phase_detection = {0};
  reset_flight_phase_detection(&flight_phase_detection);

  /*State Estimation data */
  state_est_data_t state_est_data_fsm = {0};

  /* environment data */
  env_t environment;
  env_t dummy_env;
  init_env(&dummy_env);
  init_env(&environment);

  /* Telemetry Command */
  command_e telemetry_command = IDLE_COMMAND;

  osDelay(1000);

  /* Infinite loop */
  tick_count = osKernelGetTickCount();
  tick_update = osKernelGetTickFreq() / FSM_SAMPLING_FREQ;

  while (1) {
    /* Tick Update */
    tick_count += tick_update;

    /* Read Telemetry Command */
    read_mutex(&command_mutex, &global_telemetry_command, &telemetry_command,
              sizeof(global_telemetry_command));

    /* Reset Flight Phase if Telemetry asks to */
    if (telemetry_command == CALIBRATE_SENSORS &&
        flight_phase_detection.flight_phase == IDLE) {
      reset_flight_phase_detection(&flight_phase_detection);
      telemetry_command = IDLE_COMMAND;
    }

    /* Update Local State Estimation Data */
    read_mutex(&state_est_mutex, &state_est_data_global, &state_est_data_fsm,
              sizeof(state_est_data_global));

    /* Update Local Environment Data */
    read_mutex(&env_mutex, &global_env, &environment, sizeof(global_env));

    /* get Flight Phase update */
    detect_flight_phase(tick_count, &flight_phase_detection, &state_est_data_fsm);

    /* Write updated flight Phase detection */
    if (acquire_mutex(&fsm_mutex) == osOK) {
      global_flight_phase_detection = flight_phase_detection;
      release_mutex(&fsm_mutex);
    }

    /* Log current flight phase */
    log_rocket_state(osKernelGetTickCount(), flight_phase_detection);

    /* Sleep */
    osDelayUntil(tick_count);
  }
}
