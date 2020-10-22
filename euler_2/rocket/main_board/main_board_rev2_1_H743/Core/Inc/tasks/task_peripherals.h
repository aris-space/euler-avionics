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
#define CAMERA_ON_TIME 480000

/* extern */

/* Mutexes */
extern custom_mutex_t fsm_mutex;
extern custom_mutex_t command_mutex;
extern custom_mutex_t state_est_mutex;

/* Global Data */

extern flight_phase_detection_t global_flight_phase_detection;
extern command_e global_telemetry_command;
extern state_est_data_t state_est_data_global;

extern int8_t buzzer_state;
extern int8_t camera_state;

extern TIM_HandleTypeDef htim4;

/* functions */
void vTaskPeripherals(void *argument);

#endif /* INC_TASKS_TASK_PERIPHERALS_H_ */
