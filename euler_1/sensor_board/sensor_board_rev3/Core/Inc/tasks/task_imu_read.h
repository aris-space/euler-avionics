/*
 * task_imu_read.h
 *
 *  Created on: Nov 3, 2019
 *      Author: Jonas
 */

#ifndef INC_TASK_IMU_READ_H_
#define INC_TASK_IMU_READ_H_

/* Includes */
#include "cmsis_os.h"
#include "main.h"
#include "stm32l4xx_hal.h"
#include "typedef.h"

/* Constants */
#define IMU20600_SAMPLING_FREQ 4000
#define IMU20600_SPI_TIMEOUT 30
#define IMU20600_COMMAND_LENGTH 2

/* Commands */
#define IMU20600_COMMAND_GYROSCOPE_CONFIGURATION 0x1B
#define IMU20600_COMMAND_ACCELEROMETER_CONFIGURATION1 0x1C
#define IMU20600_COMMAND_ACCELEROMETER_CONFIGURATION2 0x1D
#define IMU20600_COMMAND_FIFO_ENABLE 0x23
#define IMU20600_COMMAND_USER_CONTROL 0x6A
#define IMU20600_COMMAND_POWER_MANAGMENT1 0x6B
#define IMU20600_COMMAND_POWER_MANAGMENT2 0x6C
#define IMU20600_COMMAND_ACC_READ 0x3B
#define IMU20600_COMMAND_GYRO_READ 0x43

/* Task */
void vTaskImuRead(void *argument);

static const uint8_t GYRO_SELFTEST = 0x00;	//000b: x,y,z self-test enable
static const uint8_t GYRO_RANGE = 0x01;//00b:+-250dps, 01:500dps, 10:1000dps, 11:2000dps
static const uint8_t GYRO_FILTER = 0x02;	//10b:filter at max BW

static const uint8_t ACC_SELFTEST = 0x00;	//000b: x,y,z self-test enable
static const uint8_t ACC_RANGE = 0x03;	 	//00b:+-2g, 01:4g, 10:8g, 11:16g
static const uint8_t ACC_FIFOSIZE = 0x00;	//00b:512B, 01:1kB, 10:2kB, 11:4kB
static const uint8_t ACC_AVGFILTER = 0x00;	//00b:avg4, 01:avg8, 10:avg16, 11:avg32
static const uint8_t ACC_FILTER = 0x02;	 	//000b:1kHz, 1Xb: 4kHz

static const uint8_t GYRO_FIFO_EN = 0x00;	//0b:off, 1b:on
static const uint8_t ACC_FIFO_EN = 0x00;	 	//0b:off, 1b:on

static const uint8_t SENS_FIFO_EN = 0x00;	//0b:off, 1b:on
static const uint8_t SENS_FIFO_RST = 0x01;	//1b: reset (auto cleared)

static const uint8_t SENS_sleep_EN = 0x00;	//0b:off, 1b:on
static const uint8_t SENS_clk_src = 0x01;//001b: Auto selects best available clock source

static const uint8_t SENS_acc_axis_EN = 0x00;	//000b: x,y,z on, 111b: x,y,z off
static const uint8_t SENS_gyri_axis_EN = 0x00;	//000b: x,y,z on, 111b: x,y,z off

/* Extern */
extern SPI_HandleTypeDef hspi1;
extern osMessageQueueId_t preprocess_queue;

#endif /* INC_TASK_IMU_READ_H_ */
