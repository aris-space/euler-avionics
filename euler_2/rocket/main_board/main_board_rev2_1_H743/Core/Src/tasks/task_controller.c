/*
 * task_controller.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "../../aris-euler-controller/Inc/controller.h"
#include "util/logging_util.h"
#include "tasks/task_controller.h"
DTCM control_data_t control_data = {0};

void vTaskController(void *argument) {
  /* For periodic update */
  uint32_t tick_count, tick_update;

  state_est_data_t state_est_data_local;
  flight_phase_detection_t flight_phase_detection_local = {0};
  env_t env_local = {0};

  /* Initialize the control_data struct */

  control_init(&control_data);
  control_data.mpc_exitflag = 0;
  control_data.mpc_info.it = 0;


  osDelay(1100);

  /* Infinite loop */
  tick_count = osKernelGetTickCount();
  tick_update = osKernelGetTickFreq() / CONTROLLER_SAMPLING_FREQ;

  while (1) {
    /* Tick Update */
    tick_count += tick_update;

    /* Read mutexes */
    read_mutex(&state_est_mutex, &state_est_data_global, &state_est_data_local,
              sizeof(state_est_data_local));
    read_mutex(&fsm_mutex, &global_flight_phase_detection,
               &flight_phase_detection_local, sizeof(global_flight_phase_detection));
    read_mutex(&env_mutex, &global_env,
               &env_local, sizeof(env_local));

    /* Call the Controller */
    control_step(&control_data, &state_est_data_local, &flight_phase_detection_local, &env_local);

    /* Write Control Input into Global Variable */
    if ((acquire_mutex(&controller_mutex) == osOK) && (osKernelGetTickCount() > 120000)) {
      controller_output_global = (int32_t)(control_data.control_input * 1000);
      release_mutex(&controller_mutex);
    }

    /* Log to SD Card */
    log_controller_output(osKernelGetTickCount(), control_data);

    /* Sleep */
    osDelayUntil(tick_count);
  }
}
