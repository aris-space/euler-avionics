/*
 * logging_util.h
 *
 *  Created on: Sep 29, 2020
 *      Author: Jonas
 */

#ifndef INC_UTIL_LOGGING_UTIL_H_
#define INC_UTIL_LOGGING_UTIL_H_

#include "../aris-euler-state-estimation/Inc/state_est_const.h"
#include "cmsis_os.h"

#define LOG_BUFFER_LEN 30

#define SD_STRFMT_LEN 150

extern osMessageQueueId_t sd_queue;

typedef union {
  baro_data_t baro;
  imu_data_t imu;
  battery_data_t bat;
  gps_data_t gps;
} sensor_u;

typedef struct {
  sensor_u sensor_data;
  sensor_type_e sens_type;
  board_id_t sensor_board_id;
} sensor_log_elem_t;

typedef struct {
  int32_t controller_output;
  int32_t reference_error;
  int32_t integrated_error;
} controller_output_log_elem_t;

typedef struct {
  int32_t desired_position;
  int32_t actual_position;
} motor_log_elem_t;

typedef union {
  sensor_log_elem_t sensor_log;
  flight_phase_detection_t state;
  state_est_data_t est_var;
  controller_output_log_elem_t cont_out;
  motor_log_elem_t motor;
  char msg[LOG_BUFFER_LEN];
} log_elem_u;

typedef struct {
  log_elem_u u;
  timestamp_t ts;
  log_entry_type_e log_type;
} log_elem_t;

/* TODO [nemanja]: try increasing queue sizes to 256 at least */
#define SD_QUEUE_SIZE 256
#define FLASH_QUEUE_SIZE 128

extern osMessageQueueId_t flash_queue;

/* This might only work reliably for SD_BUFFER_LEN up to 512 */
#define SD_BUFFER_LEN 4096
/* We are writing by sectors to flash so max FLASH_BUFFER_LEN should be 4096 */
#define FLASH_BUFFER_LEN 4096

osStatus_t log_flash(void *flash_data);
osStatus_t log_sensor(timestamp_t ts, board_id_t sensor_board_id,
                     sensor_type_e sens_type, void *sensor_data);
osStatus_t log_rocket_state(timestamp_t ts,
                          flight_phase_detection_t flight_phase_detection);
osStatus_t log_estimator_var(timestamp_t ts, state_est_data_t estimator_data);
osStatus_t log_controller_output(timestamp_t ts, int32_t controller_output,
                               int32_t reference_error,
                               int32_t integrated_error);
osStatus_t log_motor(timestamp_t ts, int32_t desired_position,
                    int32_t actual_position);
osStatus_t log_msg(timestamp_t ts, char *msg);


#endif /* INC_UTIL_LOGGING_UTIL_H_ */
