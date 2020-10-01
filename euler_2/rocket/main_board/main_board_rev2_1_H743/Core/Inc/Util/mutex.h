/*
 * mutex.h
 *
 *  Created on: Jun 19, 2020
 *      Author: Jonas
 */

#ifndef INC_UTIL_MUTEX_H_
#define INC_UTIL_MUTEX_H_


#include "../aris-euler-state-estimation/Inc/state_est_const.h"
#include "util/util.h"
#include <string.h>

osStatus_t acquire_mutex(custom_mutex_t *custom_mutex);

osStatus_t release_mutex(custom_mutex_t *custom_mutex);

osStatus_t read_mutex(custom_mutex_t *custom_mutex, const void *global_data,
                      void *const local_data, int32_t size);

osStatus_t read_mutex_state_est(custom_mutex_t *custom_mutex,
                                baro_data_t *baro_data, imu_data_t *imu_data,
                                state_est_meas_t *state, uint32_t sb_number);

#endif /* INC_UTIL_MUTEX_H_ */
