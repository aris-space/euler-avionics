/*
 * task_imu_read.h
 *
 *  Created on: Nov 3, 2019
 *      Author: Jonas
 */

#ifndef INC_TASK_IMU_READ_H_
#define INC_TASK_IMU_READ_H_

/* Includes */
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include "util.h"

/* Constants */
#define IMU20601_SAMPLING_FREQ 50
#define IMU20601_SPI_TIMEOUT 3000
#define IMU20601_COMMAND_LENGTH 2

/* Commands */
#define IMU20601_COMMAND_GYROSCOPE_CONFIGURATION_WRITE 0x1B
#define IMU20601_COMMAND_ACCELEROMETER_CONFIGURATION1_WRITE 0x1C
#define IMU20601_COMMAND_ACCELEROMETER_CONFIGURATION2_WRITE 0x1D
#define IMU20601_COMMAND_FIFO_ENABLE_WRITE 0x23
#define IMU20601_COMMAND_USER_CONTROL_WRITE 0x6A
#define IMU20601_COMMAND_POWER_MANAGMENT1_WRITE 0x6B
#define IMU20601_COMMAND_POWER_MANAGMENT2_WRITE 0x6C
#define IMU20601_COMMAND_ACC_READ 0xBB
#define IMU20601_COMMAND_GYRO_READ 0xC3

/* Task */
void vTaskImuRead(void *argument);

/* Gyro Conf */
static const uint8_t GYRO_SELFTEST = 0x00;		//000b: x,y,z self-test enable
static const uint8_t GYRO_RANGE = 0x01;			//00b:+-500dps, 01:1000dps, 10:2000dps, 11:4000dps
static const uint8_t GYRO_FILTER = 0x02;		//10b:filter at max BW

/* Acc Conf */
static const uint8_t ACC_SELFTEST = 0x00;		//000b: x,y,z self-test enable
static const uint8_t ACC_RANGE = 0x03;	 		//00b:+-4g, 01:8g, 10:16g, 11:32g
static const uint8_t ACC_AVGFILTER = 0x00;		//00b:avg4, 01:avg8, 10:avg16, 11:avg32
static const uint8_t ACC_FILTER = 0x04;	 		//000b:1kHz, 1Xb: 4kHz

/* FIFO conf */
static const uint8_t GYRO_FIFO_EN = 0x00;		//0b:off, 1b:on
static const uint8_t ACC_FIFO_EN = 0x00;	 	//0b:off, 1b:on

/* User Control Conf */
static const uint8_t SENS_FIFO_EN = 0x0;		//0b:off, 1b:on
static const uint8_t I2C_DISABLE = 0x01;		//1b: disable I2C
static const uint8_t SENS_FIFO_RST = 0x01;		//1b: reset (auto cleared)

/* Power Managment 1*/
static const uint8_t reset_conf = 0x01;			//1b: reset (auto cleared)
static const uint8_t SENS_sleep_EN = 0x00;		//0b:off, 1b:on
static const uint8_t SENS_clk_src = 0x01;		//001b: Auto selects best available clock source

/* Power Managment 2 */
static const uint8_t SENS_acc_axis_EN = 0x00;	//000b: x,y,z on, 111b: x,y,z off
static const uint8_t SENS_gyri_axis_EN = 0x00;	//000b: x,y,z on, 111b: x,y,z off

/* Extern */
extern SPI_HandleTypeDef hspi1;
extern osMessageQueueId_t preprocess_queue;

#endif /* INC_TASK_IMU_READ_H_ */
