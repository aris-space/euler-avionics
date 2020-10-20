/*
 * logging_util.c
 *
 *  Created on: Sep 29, 2020
 *      Author: Jonas
 */

#include "util/logging_util.h"
#include "usbd_cdc_if.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

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

osStatus_t log_sensor(timestamp_t ts, board_id_t sensor_board_id,
                     sensor_type_e sensor_type, void *sensor_data) {
  log_elem_t log_elem = {.ts = ts,
                         .log_type = SENSOR,
                         .u.sensor_log.sensor_board_id = sensor_board_id,
                         .u.sensor_log.sens_type = sensor_type};
  switch (sensor_type) {
    case BARO:
      log_elem.u.sensor_log.sensor_data.baro = *((baro_data_t *)sensor_data);
      break;
    case IMU_1:
    case IMU_2:
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
  /* TODO [nemanja]: add log_elems to the flash queue if we are in certain states */
  osStatus_t ret_sd = osMessageQueuePut(sd_queue, &log_elem, 0U, 0U);
  //osStatus_t ret_flash = osMessageQueuePut(flash_queue, &log_elem, 0U, 0U);
  //return (ret_sd == osOK && ret_flash == osOK) ? osOK: osError;
  return ret_sd;
}

osStatus_t log_rocket_state(timestamp_t ts,
                          flight_phase_detection_t flight_phase_detection) {
  log_elem_t log_elem = {
      .ts = ts, .log_type = STATE, .u.state = flight_phase_detection};
  return osMessageQueuePut(sd_queue, &log_elem, 0U, 0U);
}

osStatus_t log_estimator_var(timestamp_t ts, state_est_data_t estimator_data, log_entry_type_e entry_log_type) {
  log_elem_t log_elem = {
      .ts = ts, .log_type = entry_log_type, .u.est_var = estimator_data};
  return osMessageQueuePut(sd_queue, &log_elem, 0U, 0U);
}

osStatus_t log_controller_output(timestamp_t ts, control_data_t control_data) {
  log_elem_t log_elem = {.ts = ts,
                         .log_type = CONTROLLER_OUTPUT,
                         .u.cont_out.control_input = (int32_t)(1000*control_data.control_input),
                         .u.cont_out.reference_error = (int32_t)(1000*control_data.reference_error),
                         .u.cont_out.integrated_error = (int32_t)(1000*control_data.integrated_error),
  	  	  	  	  	  	 .u.cont_out.sf_ref_altitude_AGL = (int32_t)(1000*control_data.sf_ref_altitude_AGL),
						 .u.cont_out.sf_velocity = (int32_t)(1000*control_data.sf_velocity),
						 .u.cont_out.ref_velocity = (int32_t)(1000*control_data.ref_velocity),
						 .u.cont_out.tracking_feedback = (int32_t)(1000*control_data.tracking_feedback),
						 .u.cont_out.lowerboundary_aw = (int32_t)(1000*control_data.lowerboundary_aw),
						 .u.cont_out.upperboundary_aw = (int32_t)(1000*control_data.upperboundary_aw),
						 .u.cont_out.exit_flag = control_data.mpc_exitflag,
						 .u.cont_out.num_iterations = control_data.mpc_info.it};
  return osMessageQueuePut(sd_queue, &log_elem, 0U, 0U);
}

osStatus_t log_motor(timestamp_t ts, int32_t desired_position,
                    int32_t actual_position) {
  log_elem_t log_elem = {.ts = ts,
                         .log_type = MOTOR_POSITION,
                         .u.motor.desired_position = desired_position,
                         .u.motor.actual_position = actual_position};
  return osMessageQueuePut(sd_queue, &log_elem, 0U, 0U);
}

osStatus_t log_msg(timestamp_t ts, char *msg) {
  log_elem_t log_elem = {.ts = ts, .log_type = MSG};
  memcpy(log_elem.u.msg, msg, strlen(msg) + 1);
  return osMessageQueuePut(sd_queue, &log_elem, 0U, 0U);
}
