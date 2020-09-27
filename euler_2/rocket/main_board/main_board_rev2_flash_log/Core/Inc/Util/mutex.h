/*
 * mutex.h
 *
 *  Created on: Jun 19, 2020
 *      Author: Jonas
 */

#ifndef INC_UTIL_MUTEX_H_
#define INC_UTIL_MUTEX_H_

#include "Util/util.h"
#include <string.h>

osStatus_t AcquireMutex(custom_mutex_t *custom_mutex);

osStatus_t ReleaseMutex(custom_mutex_t *custom_mutex);

osStatus_t ReadMutex(custom_mutex_t *custom_mutex, const void *global_data,
                     void *const local_data, int32_t size);

#endif /* INC_UTIL_MUTEX_H_ */
