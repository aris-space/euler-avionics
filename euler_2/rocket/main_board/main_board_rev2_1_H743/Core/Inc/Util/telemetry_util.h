/*
 * telemetry_util.h
 *
 *  Created on: Sep 29, 2020
 *      Author: Jonas
 */

#ifndef INC_UTIL_TELEMETRY_UTIL_H_
#define INC_UTIL_TELEMETRY_UTIL_H_

#include "Util/util.h"

/** XBEE SUPER STRUCT **/
#pragma pack(push, 1)
/* Rocket state */
typedef enum {
	IDLE_T = 1,
	AIRBRAKE_TEST_T,
	THRUSTING_T,
	COASTING_T,
	CONTROL_T,
	BIAS_RESET_T,
	APOGEE_APPROACH_T,
	DROGUE_DESCENT_T,
	BALLISTIC_DESCENT_T,
	MAIN_DESCENT_T,
	TOUCHDOWN_T
} flight_phase_telemetry_e;

/* GPS data */
typedef struct {
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
    uint32_t lat_decimal;
    uint32_t lon_decimal;
    uint16_t HDOP;
    uint16_t altitude;
    uint8_t satellite;
    uint8_t lat_deg;
    uint8_t lon_deg;
    uint8_t fix;
} gps_data_telemetry_t;

/* Battery Data */
typedef struct {
    uint16_t battery;
    uint16_t current;
    uint16_t consumption;
} telemetry_battery_data_t;

/* SB Data */
typedef struct {
    int32_t pressure;
    int32_t temperature;
    int16_t gyro_x, gyro_y, gyro_z;
    int16_t acc_x, acc_y, acc_z;
} telemetry_sb_data_t;

typedef struct {
	timestamp_t ts;
    telemetry_sb_data_t sb_data;
    telemetry_battery_data_t battery;
    gps_data_telemetry_t gps;
    int32_t height;
    int32_t velocity;
    int32_t airbrake_extension;
    flight_phase_telemetry_e flight_phase;
} telemetry_t;
#pragma pack(pop)

#endif /* INC_UTIL_TELEMETRY_UTIL_H_ */
