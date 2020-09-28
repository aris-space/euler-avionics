/*
 * task_controller.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "tasks/task_controller.h"

/* Abbreviation 'aw' is used to described everything related to the antiwindup
 */

void vTaskController(void *argument) {
  /* For periodic update */
  uint32_t tick_count, tick_update;

  state_est_data_t state_est_data_local;
  flight_phase_detection_t current_flight_phase_detection = {0};

  /* Initialize the control_data struct */
  control_data_t control_data = {0};
  control_data_init(&control_data);

  osDelay(1100);

  /* Infinite loop */
  tick_count = osKernelGetTickCount();
  tick_update = osKernelGetTickFreq() / CONTROLLER_SAMPLING_FREQ;

  while (1) {
    /* Tick Update */
    tick_count += tick_update;

    /* Update Sensor Fusion Variables */
    ReadMutex(&state_est_mutex, &state_est_data_global, &state_est_data_local,
              sizeof(state_est_data_local));

    control_data.sf_ref_altitude_AGL =
        ((float)state_est_data_global.position_world[2]) / 1000;
    control_data.sf_velocity =
        ((float)state_est_data_global.velocity_world[2]) / 1000;

    /* Update flight Phase */
    ReadMutex(&fsm_mutex, &global_flight_phase_detection,
              &current_flight_phase_detection, sizeof(state_est_data_local));

    /** MAKE SURE THE RIGHT CONTROLLER IS ACTIVE IS ACTIVE!!!!! **/
    if (LQR_ACTIVE) {
      compute_control_input(&control_data, &current_flight_phase_detection);
    } else {
      compute_test_control_input(&control_data);
    }

    /* Write Control Input into Global Variable */
    if (AcquireMutex(&controller_mutex) == osOK) {
      controller_output_global = (int32_t)(control_data.control_input * 1000);
      ReleaseMutex(&controller_mutex);
    }

    /* Log to SD Card */
    logControllerOutput(osKernelGetTickCount(),
                        (int32_t)(control_data.control_input * 1000),
                        (int32_t)(control_data.reference_error * 1000),
                        (int32_t)(control_data.integrated_error * 1000));

    /* Sleep */
    osDelayUntil(tick_count);
  }
}
