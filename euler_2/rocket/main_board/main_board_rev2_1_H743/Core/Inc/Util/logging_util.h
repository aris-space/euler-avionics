/*
 * logging_util.h
 *
 *  Created on: Sep 29, 2020
 *      Author: Jonas
 */

#ifndef INC_UTIL_LOGGING_UTIL_H_
#define INC_UTIL_LOGGING_UTIL_H_

#include "cmsis_os.h"
#include "Sim_Con/state_est_const.h"

#define LOG_BUFFER_LEN 30

#define SD_STRFMT_LEN 150

#define LOG_QUEUE_SIZE 128
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

#define SD_QUEUE_SIZE 128
#define FLASH_QUEUE_SIZE 128

extern osMessageQueueId_t flash_queue;

/* This might only work reliably for SD_BUFFER_LEN up to 512 */
#define SD_BUFFER_LEN 4096
/* We are writing by sectors to flash so max FLASH_BUFFER_LEN should be 4096 */
#define FLASH_BUFFER_LEN 4096

// typedef struct {
//	char str[LOG_BUFFER_LEN];
//} log_entry_t;

osStatus_t logFlash(void *flash_data);
osStatus_t logSensor(timestamp_t ts, board_id_t sensor_board_id,
                     sensor_type_e sens_type, void *sensor_data);
osStatus_t logRocketState(timestamp_t ts,
                          flight_phase_detection_t flight_phase_detection);
osStatus_t logEstimatorVar(timestamp_t ts, state_est_data_t estimator_data);
osStatus_t logControllerOutput(timestamp_t ts, int32_t controller_output,
                               int32_t reference_error,
                               int32_t integrated_error);
osStatus_t logMotor(timestamp_t ts, int32_t desired_position,
                    int32_t actual_position);
osStatus_t logMsg(timestamp_t ts, char *msg);


#endif /* INC_UTIL_LOGGING_UTIL_H_ */
