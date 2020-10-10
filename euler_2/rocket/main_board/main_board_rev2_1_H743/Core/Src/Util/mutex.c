/*
 * mutex.c
 *
 *  Created on: Jun 19, 2020
 *      Author: Jonas
 */
#include "../aris-euler-state-estimation/Inc/flight_phase_detection.h"
#include "util/mutex.h"

osStatus_t acquire_mutex(custom_mutex_t *custom_mutex) {
  osStatus_t status = osOK;
  status = osMutexAcquire(custom_mutex->mutex, 10);
  if (status == osOK) {
    custom_mutex->counter++;
  }

  return status;
}

osStatus_t release_mutex(custom_mutex_t *custom_mutex) {
  return osMutexRelease(custom_mutex->mutex);
}

osStatus_t read_mutex(custom_mutex_t *custom_mutex, void const *global_data,
                     void *const local_data, int32_t size) {
  uint8_t buffer[100] = {0};
  uint32_t counter = custom_mutex->counter;
  osStatus_t status = osError;
  for (int i = 0; i < 5; i++) {
    memcpy(&buffer[0], global_data, size);
    if (custom_mutex->counter == counter) {
      memcpy(local_data, buffer, size);
      status = osOK;
      break;
    }
    counter = custom_mutex->counter;
  }
  return status;
}

osStatus_t read_mutex_state_est(custom_mutex_t *custom_mutex, state_est_meas_t *state, sb_data_t *data,
                                uint32_t sb_number) {
  /* Buffer */
  uint32_t Placeholder_timestamps[3] = {0};
  float Placeholder_measurement[4] = {0};

  /* Status */
  osStatus_t status = osError;

  /* Counter */
  uint32_t counter = custom_mutex->counter;

  for (int i = 0; i < 5; i++) {
    /* Write in Buffer */
    Placeholder_measurement[0] = (float)(data->baro.pressure);
    Placeholder_timestamps[0] = data->baro.ts;
    Placeholder_measurement[1] = ((float)(data->imu_1.acc_z)) / 1024;
    Placeholder_timestamps[1] = data->imu_1.ts;
    Placeholder_measurement[2] = ((float)(data->imu_2.acc_z)) / 1024;
    Placeholder_timestamps[2] = data->imu_2.ts;
    Placeholder_measurement[3] = ((float)(data->baro.temperature)) / 100;

    /* Check if Mutex was aquired */
    if (custom_mutex->counter == counter) {
      state->baro_data[sb_number - 1].pressure = Placeholder_measurement[0];
      state->baro_data[sb_number - 1].temperature = Placeholder_measurement[3];
      state->baro_data[sb_number - 1].ts = Placeholder_timestamps[0];

      state->imu_data[2*(sb_number - 1)].acc_x =
          Placeholder_measurement[1] * GRAVITATION;
      state->imu_data[2*(sb_number - 1)].ts = Placeholder_timestamps[1];
      state->imu_data[2*(sb_number - 1) + 1].acc_x =
          Placeholder_measurement[2] * GRAVITATION;
      state->imu_data[2*(sb_number - 1) + 1].ts = Placeholder_timestamps[2];
      status = osOK;
      break;
    }
    counter = custom_mutex->counter;
  }

  return status;
}
