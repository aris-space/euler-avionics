/*
 * task_peripherals.h
 *
 *  Created on: Jun 15, 2020
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_XBEE_H_
#define INC_TASKS_TASK_XBEE_H_

/* includes */

#include "../aris-euler-state-estimation/Inc/flight_phase_detection.h"
#include "util/mutex.h"
#include "util/util.h"

/* defines */
#define PERIPHERALS_SAMPLING_FREQ 10

/* extern */

/* Mutexes */
extern custom_mutex_t fsm_mutex;
extern custom_mutex_t command_mutex;

/* Global Data */

extern flight_phase_detection_t global_flight_phase_detection;
extern command_e global_telemetry_command;

extern TIM_HandleTypeDef htim4;

/* functions */
void vTaskPeripherals(void *argument);

#endif /* INC_TASKS_TASK_PERIPHERALS_H_ */
