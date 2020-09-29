/*
 * logging_util.c
 *
 *  Created on: Sep 29, 2020
 *      Author: Jonas
 */

#include "usbd_cdc_if.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "Util/logging_util.h"

/*
 *  %c	character
 %d	decimal (integer) number (base 10)
 %e	exponential floating-point number
 %f	floating-point number
 %i	integer (base 10)
 %o	octal number (base 8)
 %s	a string of characters
 %u	unsigned decimal (integer) number
 %x	number in hexadecimal (base 16)
 %%	print a percent sign
 \%	print a percent sign
 */

osStatus_t logSensor(timestamp_t ts, board_id_t sensor_board_id,
                     sensor_type_e sensor_type, void *sensor_data) {
  log_elem_t log_elem = {.ts = ts,
                         .log_type = SENSOR,
                         .u.sensor_log.sensor_board_id = sensor_board_id,
                         .u.sensor_log.sens_type = sensor_type};
  switch (sensor_type) {
    case BARO:
      log_elem.u.sensor_log.sensor_data.baro = *((baro_data_t *)sensor_data);
      break;
    case IMU:
      log_elem.u.sensor_log.sensor_data.imu = *((imu_data_t *)sensor_data);
      break;
    case GPS:
      log_elem.u.sensor_log.sensor_data.gps = *((gps_data_t *)sensor_data);
      break;
    case BATTERY:
      log_elem.u.sensor_log.sensor_data.bat = *((battery_data_t *)sensor_data);
      break;
    default:
      return osError; /* bad sensor type provided, exit the function */
  }
  osStatus_t ret_sd = osMessageQueuePut(sd_queue, &log_elem, 0U, 0U);
  // osStatus_t ret_flash = osMessageQueuePut(flash_queue, &log_elem, 0U, 0U);
  // return (ret_sd == osOK && ret_flash == osOK) ? osOK: osError;
  return ret_sd;
}

osStatus_t logRocketState(timestamp_t ts,
                          flight_phase_detection_t flight_phase_detection) {
  log_elem_t log_elem = {
      .ts = ts, .log_type = STATE, .u.state = flight_phase_detection};
  return osMessageQueuePut(sd_queue, &log_elem, 0U, 0U);
}

osStatus_t logEstimatorVar(timestamp_t ts, state_est_data_t estimator_data) {
  log_elem_t log_elem = {
      .ts = ts, .log_type = ESTIMATOR_VAR, .u.est_var = estimator_data};
  return osMessageQueuePut(sd_queue, &log_elem, 0U, 0U);
}

osStatus_t logControllerOutput(timestamp_t ts, int32_t controller_output,
                               int32_t reference_error,
                               int32_t integrated_error) {
  log_elem_t log_elem = {.ts = ts,
                         .log_type = CONTROLLER_OUTPUT,
                         .u.cont_out.controller_output = controller_output,
                         .u.cont_out.reference_error = reference_error,
                         .u.cont_out.integrated_error = integrated_error};
  return osMessageQueuePut(sd_queue, &log_elem, 0U, 0U);
}

osStatus_t logMotor(timestamp_t ts, int32_t desired_position,
                    int32_t actual_position) {
  log_elem_t log_elem = {.ts = ts,
                         .log_type = MOTOR_POSITION,
                         .u.motor.desired_position = desired_position,
                         .u.motor.actual_position = actual_position};
  return osMessageQueuePut(sd_queue, &log_elem, 0U, 0U);
}

osStatus_t logMsg(timestamp_t ts, char *msg) {
  log_elem_t log_elem = {.ts = ts, .log_type = MSG};
  memcpy(log_elem.u.msg, msg, strlen(msg) + 1);
  return osMessageQueuePut(sd_queue, &log_elem, 0U, 0U);
}
