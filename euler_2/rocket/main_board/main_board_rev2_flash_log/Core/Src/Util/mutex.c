/*
 * mutex.c
 *
 *  Created on: Jun 19, 2020
 *      Author: Jonas
 */
#include "Util/mutex.h"

osStatus_t AcquireMutex(custom_mutex_t *custom_mutex){
	osStatus_t status = osOK;
	status = osMutexAcquire(custom_mutex->mutex, 10);
	if(status == osOK){
		custom_mutex->counter++;
	}

	return status;
}

osStatus_t ReleaseMutex(custom_mutex_t *custom_mutex){
	return osMutexRelease(custom_mutex->mutex);
}

osStatus_t ReadMutex(custom_mutex_t *custom_mutex,void const* global_data, void* const local_data, int32_t size){
	uint8_t buffer[100] = { 0 };
	uint32_t counter = custom_mutex->counter;
	osStatus_t status = osError;
	for(int i = 0; i < 5; i++){
		memcpy(&buffer[0], global_data, size);
		if(custom_mutex->counter == counter){
			memcpy(local_data, buffer, size);
			status = osOK;
			break;
		}
		counter = custom_mutex->counter;
	}
	return status;
}
