/*
 * telemetry_util.h
 *
 *  Created on: Sep 29, 2020
 *      Author: Jonas
 */

#ifndef INC_UTIL_TELEMETRY_UTIL_H_
#define INC_UTIL_TELEMETRY_UTIL_H_

/** XBEE SUPER STRUCT **/

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
  uint8_t startbyte;
  telemetry_sb_data_t sb_data;
  telemetry_battery_data_t battery;
  gps_data_t gps;
  int32_t height;
  int32_t velocity;
  int32_t airbrake_extension;
  flight_phase_e flight_phase;
  timestamp_t ts;
  uint8_t checksum;
} telemetry_t;

#endif /* INC_UTIL_TELEMETRY_UTIL_H_ */
