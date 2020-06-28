/*
 * state_est_settings.h
 *
 *  Created on: Jun 12, 2020
 *      Author: Jonas
 */

#ifndef INC_SIM_CON_STATE_EST_SETTINGS_H_
#define INC_SIM_CON_STATE_EST_SETTINGS_H_


#include "Util/util.h"


/* CONSTANTS FOR CONFIGURATION OF THE STATE ESTIMATION */
#define STATE_ESTIMATION_FREQUENCY 100

#define NUM_SENSORBOARDS 3 /* Number of Sensor Boards */
#define MAX_NUM_SENSORS /* Max Betwee Num IMU and Num BARO */
#define NUM_IMU 3
#define NUM_BARO 3
#define MAX_LENGTH_ROLLING_MEMORY 18
#define USE_SENSOR_ELIMINATION_BY_EXTRAPOLATION 0 // set to 1 to activate sensor elimination by extrapolation for barometer and temperature
#define EXTRAPOLATION_POLYFIT_DEGREE 2
#define LAMBDA 0.0001		/* Lambda for Moore Penrose Pseudoinverse */
#define PI 3.1415

/* State Estimation Baro struct */
typedef struct {
	float pressure;
	float temperature;
	timestamp_t ts;
}baro_state_est_t;


/* State Estimation IMU struct */
typedef struct {
	float gyro_x, gyro_y, gyro_z;
	float acc_x, acc_y, acc_z;
	timestamp_t ts;
}imu_state_est_t;

/* State Estimation combined Data struct */
typedef struct {
    /* pressure in [Pa] and temperature in [°C] */
	baro_state_est_t baro_data[NUM_SENSORBOARDS];
    /* acceleration in [m/s^2] and angular velocity in [rad/s] */
    /* all in rocket frame where x-dir is along length of rocket */
	imu_state_est_t imu_data[NUM_SENSORBOARDS];
} state_est_meas_t;

#endif /* INC_SIM_CON_STATE_EST_SETTINGS_H_ */
