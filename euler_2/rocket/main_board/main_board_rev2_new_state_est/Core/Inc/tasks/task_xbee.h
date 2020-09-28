/*
 * task_xbee.h
 *
 *  Created on: Jun 15, 2020
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_XBEE_H_
#define INC_TASKS_TASK_XBEE_H_

/* includes */

#include "main.h"
#include "Util/util.h"
#include "Sim_Con/env.h"
#include "Util/mutex.h"

/* defines */
#define XBEE_SAMPLING_FREQ 10
#define XBEE_SAMPLING_FREQ_HIGH 100

/* extern */

/* Mutexes */
extern custom_mutex_t fsm_mutex;
extern custom_mutex_t state_est_mutex;
extern custom_mutex_t command_mutex;
extern custom_mutex_t sb1_mutex;
extern custom_mutex_t sb2_mutex;
extern custom_mutex_t sb3_mutex;
extern custom_mutex_t motor_mutex;
extern custom_mutex_t battery_mutex;
extern custom_mutex_t gps_mutex;

/* Global Data */
extern baro_data_t sb1_baro;
extern imu_data_t sb1_imu;
extern baro_data_t sb2_baro;
extern imu_data_t sb2_imu;
extern baro_data_t sb3_baro;
extern imu_data_t sb3_imu;
extern state_est_data_t state_est_data_global;
extern int32_t global_airbrake_extension;
extern flight_phase_detection_t global_flight_phase_detection;
extern command_e global_telemetry_command;
extern gps_data_t globalGPS;
extern telemetry_battery_data_t global_battery_data;

extern UART_HandleTypeDef huart7;

/* functions */
void vTaskXbee(void *argument);

#endif /* INC_TASKS_TASK_XBEE_H_ */
