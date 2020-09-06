/*
 * util.h
 *
 *  Created on: Feb 24, 2020
 *      Author: stoja
 */

#ifndef INC_UTIL_UTIL_H_
#define INC_UTIL_UTIL_H_

#include "cmsis_os.h"

/** BASIC TYPES **/

/* Timestamp */
typedef uint32_t timestamp_t;

/* Board ID */
typedef uint8_t board_id_t;

/** ENUMS **/

typedef enum {
	IDLE_COMMAND = 155,
	CALIBRATE_SENSORS = 73,
	AIRBRAKE_TEST_COMMAND = 217,
	TELEMETRY_HIGH_SAMPLING = 13,
	TELEMETRY_LOW_SAMPLING = 197,
	ENABLE_BUZZER = 113,
	DISABLE_SELF_HOLD = 251,
	ENABLE_CAMERA = 2
} command_e;

typedef struct {
	command_e command[4];
} command_xbee_t;

/* Rocket state */
typedef enum {
	IDLE = 1, AIRBRAKE_TEST, THRUSTING, COASTING, DESCENT, RECOVERY
} flight_phase_e;

/* Mach Regime */
typedef enum {
	SUBSONIC = 1, TRANSONIC, SUPERSONIC
} mach_regime_e;

/* Sensor type */
typedef enum {
	BARO = 1, IMU, GPS, BATTERY
} sensor_type_e;

/* Log entry */
typedef enum {
	SENSOR = 1, STATE, ESTIMATOR_VAR, CONTROLLER_OUTPUT, MOTOR_POSITION, MSG
} log_entry_type_e;

/** SENSOR DATA TYPES **/

/* IMU data */
typedef struct {
	int16_t gyro_x, gyro_y, gyro_z;
	int16_t acc_x, acc_y, acc_z;
	timestamp_t ts;
} imu_data_t;

/* Barometer data */
typedef struct {
	int32_t pressure;
	int32_t temperature;
	timestamp_t ts;
} baro_data_t;

typedef struct {
	baro_data_t baro;
	imu_data_t imu;
	uint8_t checksum;
} sb_data_t;

/* GPS data */
typedef struct {
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
	uint8_t satellite;
	uint8_t lat_deg;
	uint32_t lat_decimal;
	uint8_t lon_deg;
	uint32_t lon_decimal;
	uint8_t fix;
	uint16_t HDOP;
	uint16_t altitude;
} gps_data_t;

/* Battery Data */
typedef struct {
	uint16_t supply;
	uint16_t battery;
	uint16_t current;
	uint16_t consumption;
	uint16_t power;
} battery_data_t;

/** CONTROL DATA TYPES **/

/* State Estimation Output */
typedef struct {
	int32_t position_world[3];
	int32_t velocity_rocket[3];
	int32_t acceleration_rocket[3];
	int32_t velocity_world[3];
	int32_t acceleration_world[3];
} state_est_data_t;

/* FSM States */
typedef struct {
	flight_phase_e flight_phase;
	mach_regime_e mach_regime;
	float mach_number;
	int8_t num_samples_positive;
} flight_phase_detection_t;

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

/* Sensor Board Mutexes */
typedef struct {
	osMutexId_t mutex;
	uint32_t counter;
} custom_mutex_t;

static const command_xbee_t IDLE_XBEE_DATA = { { IDLE_COMMAND, IDLE_COMMAND,
		IDLE_COMMAND, IDLE_COMMAND } };

static const imu_data_t EMPTY_IMU = { 0 };

/** LOGGING **/

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
	board_id_t sensor_board_id;
	sensor_type_e sens_type;
	sensor_u sensor_data;
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
	timestamp_t ts;
	log_entry_type_e log_type;
	log_elem_u u;
} log_elem_t;

#define SD_QUEUE_SIZE 128
#define FLASH_QUEUE_SIZE 128

extern osMessageQueueId_t flash_queue;

/* This might only work reliably for SD_BUFFER_LEN up to 512 */
#define SD_BUFFER_LEN 4096
/* We are writing by sectors to flash so max FLASH_BUFFER_LEN should be 4096 */
#define FLASH_BUFFER_LEN 4096

//typedef struct {
//	char str[LOG_BUFFER_LEN];
//} log_entry_t;

osStatus_t logFlash(void *flash_data);
osStatus_t logSensor(timestamp_t ts, board_id_t sensor_board_id,
		sensor_type_e sens_type, void *sensor_data);
osStatus_t logRocketState(timestamp_t ts,
		flight_phase_detection_t flight_phase_detection);
osStatus_t logEstimatorVar(timestamp_t ts, state_est_data_t estimator_data);
osStatus_t logControllerOutput(timestamp_t ts, int32_t controller_output,
		int32_t reference_error, int32_t integrated_error);
osStatus_t logMotor(timestamp_t ts, int32_t desired_position,
		int32_t actual_position);
osStatus_t logMsg(timestamp_t ts, char *msg);

/* USB Fake data insert */

#define USB_DATA_ENABLE 0

/** USB DEBUGGING **/

/* Debug flag */
#ifdef DEBUG
#undef DEBUG
#endif
/* Comment the next line in order to disable debug mode */
#define DEBUG

#ifdef DEBUG
osMutexId_t print_mutex;
#define PRINT_BUFFER_LEN 300
char print_buffer[PRINT_BUFFER_LEN];
#endif

uint8_t UsbPrint(const char *format, ...);

#endif /* INC_UTIL_UTIL_H_ */



//for (;;
//		) {
//			if (osMessageQueueGet(logf_sector_queue, &sector_id, NULL,
//					osWaitForever) == osOK) {
//				UsbPrint("[STORAGE TASK] Logging sector: %hu\n", sector_id);
//				W25qxx_ReadSector(logf_buffer_read, sector_id, 0, FLASH_BUFFER_LEN);
//				uint16_t logf_buffer_read_idx = 0;
//				uint16_t log_elem_size = 0;
//				char print_buf[200] = {0};
//				while (logf_buffer_read_idx < FLASH_BUFFER_LEN) {
//					uint8_t log_type_uint8_t =
//					logf_buffer_read[logf_buffer_read_idx++];
//					log_entry_type_e log_type =
//					(log_entry_type_e) log_type_uint8_t;
//					switch (log_type) {
//						case SENSOR: {
//							log_elem_size = sizeof(sensor_log_elem_t);
//							sensor_log_elem_t sensor_log;
//							memcpy(&sensor_log,
//							&logf_buffer_read[logf_buffer_read_idx],
//							log_elem_size);
//							snprintf(print_buf + strlen(print_buf), 200,
//							"[FLASH]: Sector#: %hu, Page#: %hu; Sensor data: %lu;%d;%hi,%d,",
//							sector_id, logf_buffer_read_idx / 256,
//							sensor_log.ts, SENSOR,
//							sensor_log.sensor_board_id,
//							sensor_log.sens_type);
//							switch (sensor_log.sens_type) {
//								case BARO: {
//									baro_data_t *baro_data_ptr =
//									(baro_data_t*) &sensor_log.sensor_data.baro;
//									snprintf(print_buf + strlen(print_buf), 200,
//									"P: %ld,T: %ld,Ts: %lu\n",
//									baro_data_ptr->pressure,
//									baro_data_ptr->temperature,
//									baro_data_ptr->ts);
//								}
//								break;
//								case IMU: {
//									imu_data_t *imu_data_ptr =
//									(imu_data_t*) &sensor_log.sensor_data.imu;
//									snprintf(print_buf + strlen(print_buf), 200,
//									"Ax: %hd, Ay: %hd, Az: %hd,Gx: %hd,Gy: %hd,Gz: %hd,Ts: %lu\n",
//									imu_data_ptr->acc_x,
//									imu_data_ptr->acc_y,
//									imu_data_ptr->acc_z,
//									imu_data_ptr->gyro_x,
//									imu_data_ptr->gyro_y,
//									imu_data_ptr->gyro_z, imu_data_ptr->ts);
//								}
//								break;
//								case BATTERY: {
//									battery_data_t *battery_data_ptr =
//									(battery_data_t*) &sensor_log.sensor_data.bat;
//									snprintf(print_buf + strlen(print_buf), 200,
//									"%hd,%hd,%hd,%hd\n",
//									battery_data_ptr->battery,
//									battery_data_ptr->consumption,
//									battery_data_ptr->current,
//									battery_data_ptr->supply);
//									break;
//								}
//								default: {
//									snprintf(print_buf + strlen(print_buf), 200,
//									"[FLASH] Sensor type not recognized!\n");
//									break;
//								}
//							}
//						}
//						break;
//						case MSG: {
//							// first elem of log msg is its size
//							log_elem_size = logf_buffer_read[logf_buffer_read_idx++];
//							char str_log[LOG_BUFFER_LEN];
//							memcpy(&str_log,
//							&logf_buffer_read[logf_buffer_read_idx],
//							log_elem_size);
//							snprintf(print_buf + strlen(print_buf), 200,
//							"[FLASH]: Sector#: %hu, Page#: %hu; Strlen: %d, Msg: %s\n",
//							sector_id, logf_buffer_read_idx / 256,
//							log_elem_size, str_log);
//						}
//						break;
//						default: {
//							logf_buffer_read_idx = FLASH_BUFFER_LEN;
//							snprintf(print_buf + strlen(print_buf), 200,
//							"[FLASH] Log type not recognized while reading!\n");
//						}
//					}
//					logf_buffer_read_idx += log_elem_size;
//					UsbPrint(print_buf);
//					print_buf[0] = 0;
//				}
//			}
//		}
