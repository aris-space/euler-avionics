/*
 * task_peripherals.h
 *
 *  Created on: Jun 15, 2020
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_XBEE_H_
#define INC_TASKS_TASK_XBEE_H_


/* includes */

#include "main.h"
#include "Util/util.h"
#include "Sim_Con/flight_phase_detection.h"
#include "Util/mutex.h"

/* defines */
#define PERIPHERALS_SAMPLING_FREQ 10

#define CAMERA_ON 3500
#define CAMERA_WAIT 1000
#define CAMERA_TRIGGER 200

/* extern */

/* Mutexes */
extern custom_mutex_t fsm_mutex;
extern custom_mutex_t command_mutex;


/* Global Data */

extern flight_phase_detection_t global_flight_phase_detection;
extern command_e global_telemetry_command;


/* functions */
void vTaskPeripherals(void *argument);

#endif /* INC_TASKS_TASK_PERIPHERALS_H_ */
