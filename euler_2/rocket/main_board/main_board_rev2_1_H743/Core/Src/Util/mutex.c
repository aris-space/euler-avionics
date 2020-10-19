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
  uint8_t buffer[100];
  volatile uint32_t counter = custom_mutex->counter;
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
  uint32_t Placeholder_timestamps[3];

  #if STATE_ESTIMATION_TYPE == 1
    float Placeholder_measurement[4];
  #elif STATE_ESTIMATION_TYPE == 2
    float Placeholder_measurement[14];
  #endif

  /* Status */
  osStatus_t status = osError;

  /* Counter */
  volatile uint32_t counter = custom_mutex->counter;

  for (int i = 0; i < 5; i++) {
    /* Write in Buffer */
    Placeholder_timestamps[0] = data->baro.ts;
    Placeholder_timestamps[1] = data->imu_1.ts;
    Placeholder_timestamps[2] = data->imu_2.ts;

    Placeholder_measurement[0] = (float)(data->baro.pressure);
    Placeholder_measurement[1] = ((float)(data->baro.temperature)) / 100;    

    #if STATE_ESTIMATION_TYPE == 1
      Placeholder_measurement[2] = ((float)(data->imu_1.acc_z)) / 1024 * (-GRAVITATION);
      Placeholder_measurement[3] = ((float)(data->imu_2.acc_z)) / 1024 * (-GRAVITATION);

    #elif STATE_ESTIMATION_TYPE == 2
      Placeholder_measurement[2] = ((float)(data->imu_1.acc_x)) / 1024 * GRAVITATION;
      Placeholder_measurement[3] = ((float)(data->imu_1.acc_y)) / 1024 * GRAVITATION;
      Placeholder_measurement[4] = ((float)(data->imu_1.acc_z)) / 1024 * GRAVITATION;
      Placeholder_measurement[5] = ((float)(data->imu_1.gyro_x)) / 16.4 / 180 * M_PI;
      Placeholder_measurement[6] = ((float)(data->imu_1.gyro_y)) / 16.4 / 180 * M_PI;
      Placeholder_measurement[7] = ((float)(data->imu_1.gyro_z)) / 16.4 / 180 * M_PI;
      Placeholder_measurement[8] = ((float)(data->imu_2.acc_x)) / 1024 * GRAVITATION;
      Placeholder_measurement[9] = ((float)(data->imu_2.acc_y)) / 1024 * GRAVITATION;
      Placeholder_measurement[10] = ((float)(data->imu_2.acc_z)) / 1024 * GRAVITATION;
      Placeholder_measurement[11] = ((float)(data->imu_2.gyro_x)) / 16.4 / 180 * M_PI;
      Placeholder_measurement[12] = ((float)(data->imu_2.gyro_y)) / 16.4 / 180 * M_PI;
      Placeholder_measurement[13] = ((float)(data->imu_2.gyro_z)) / 16.4 / 180 * M_PI;
    #endif

    /* Check if Mutex was aquired */
    if (custom_mutex->counter == counter) {
      state->baro_data[sb_number - 1].ts = Placeholder_timestamps[0];
      state->imu_data[2*(sb_number - 1)].ts = Placeholder_timestamps[1];
      state->imu_data[2*(sb_number - 1) + 1].ts = Placeholder_timestamps[2];

      state->baro_data[sb_number - 1].pressure = Placeholder_measurement[0];
      state->baro_data[sb_number - 1].temperature = Placeholder_measurement[1];
      
      #if STATE_ESTIMATION_TYPE == 1
        state->imu_data[2*(sb_number - 1)].acc_x = Placeholder_measurement[2];
        state->imu_data[2*(sb_number - 1) + 1].acc_x = Placeholder_measurement[3];

      #elif STATE_ESTIMATION_TYPE == 2
        state->imu_data[2*(sb_number - 1)].acc_x = Placeholder_measurement[2];
        state->imu_data[2*(sb_number - 1)].acc_y = Placeholder_measurement[3];
        state->imu_data[2*(sb_number - 1)].acc_z = Placeholder_measurement[4];
        state->imu_data[2*(sb_number - 1)].gyro_x = Placeholder_measurement[5];
        state->imu_data[2*(sb_number - 1)].gyro_y = Placeholder_measurement[6];
        state->imu_data[2*(sb_number - 1)].gyro_z = Placeholder_measurement[7];
        state->imu_data[2*(sb_number - 1) + 1].acc_x = Placeholder_measurement[8];
        state->imu_data[2*(sb_number - 1) + 1].acc_y = Placeholder_measurement[9];
        state->imu_data[2*(sb_number - 1) + 1].acc_z = Placeholder_measurement[10];
        state->imu_data[2*(sb_number - 1) + 1].gyro_x = Placeholder_measurement[11];
        state->imu_data[2*(sb_number - 1) + 1].gyro_y = Placeholder_measurement[12];
        state->imu_data[2*(sb_number - 1) + 1].gyro_z = Placeholder_measurement[13];
      #endif
      
      status = osOK;
      break;
    }
    counter = custom_mutex->counter;
  }

  return status;
}
