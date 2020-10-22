/*
 * util.h
 *
 *  Created on: Feb 24, 2020
 *      Author: stoja
 */

#ifndef INC_UTIL_UTIL_H_
#define INC_UTIL_UTIL_H_

#include "cmsis_os.h"
#include "stm32h7xx.h"

/** BASIC TYPES **/

/* DO NOT USE DTCM FOR DMA!! */
/* DO NOT USE DTCM WITH INITALIZED VALUES!! */
/* ONLY USE DTCM ON GLOBAL VARIABLES */
#define DTCM \
      __attribute__((section(".user_dtcm")))

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
  ENABLE_CAMERA = 222,
  DISABLE_CAMERA = 123
} command_e;

typedef struct {
  command_e command[4];
} command_xbee_t;

/* Sensor type */
typedef enum { BARO = 1, IMU_1, IMU_2, GPS, BATTERY } sensor_type_e;

/* Log entry */
typedef enum {
  SENSOR = 1,
  STATE,
  ESTIMATOR_VAR_1D,
  CONTROLLER_OUTPUT,
  MOTOR_POSITION,
  MSG
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
  imu_data_t imu_1;
  imu_data_t imu_2;
  uint8_t checksum;
} sb_data_t;

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

/* ARE FOUND IN STATE_EST_CONST.h */


/* Custom Mutexes */
typedef struct {
  osMutexId_t mutex;
  uint32_t counter;
} custom_mutex_t;

static const command_xbee_t IDLE_XBEE_DATA = {
    {IDLE_COMMAND, IDLE_COMMAND, IDLE_COMMAND, IDLE_COMMAND}};

static const imu_data_t EMPTY_IMU = {0};

/* USB Fake data insert */

//#define USB_DATA_ENABLE

/** USB DEBUGGING **/

/* Debug flag */
#ifdef DEBUG
#undef DEBUG
#endif
/* Comment the next line in order to disable debug mode -- should be disabled
 * during flight */
//#define DEBUG
/* Flash read flag */
#ifdef FLASH_READ
#undef FLASH_READ
#endif
/* Comment the next line in order to disable flash read -- should be disabled
 * during flight */
// #define FLASH_READ

#ifdef DEBUG
osMutexId_t print_mutex;
#define PRINT_BUFFER_LEN 300
char print_buffer[PRINT_BUFFER_LEN];
#endif

uint8_t usb_print(const char *format, ...);

#endif /* INC_UTIL_UTIL_H_ */
