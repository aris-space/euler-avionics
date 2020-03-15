/*
 * task_baro_read.h
 *
 *  Created on: Nov 1, 2019
 *      Author: Jonas
 */

#ifndef INC_TASK_BARO_READ_H_
#define INC_TASK_BARO_READ_H_

/* Includes */
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include "util.h"
#include "typedef.h"

/* Constants */
#define BARO_ADDRESS 0xEE
#define BARO_SAMPLING_FREQ 50
#define BARO_I2C_TIMEOUT 10

#define BARO_COMMAND_LENGTH 1
#define BARO_COEFFICIENT_LENGTH 2
#define BARO_DATA_LENGTH 3

/* Parameters */
#define BARO_OSR 0 		/* = 0 -> OSR of 256, = 1 -> OSR of 512, = 2 -> OSR of 1024, = 3 -> OSR on 2048, = 4 -> OSR of 4096 */

/* Commands */
#define BARO_COMMAND_RESET 0x1E
#define BARO_COMMAND_CONVERT_D1_OSR_256 0x40
#define BARO_COMMAND_CONVERT_D1_OSR_512 0x42
#define BARO_COMMAND_CONVERT_D1_OSR_1024 0x44
#define BARO_COMMAND_CONVERT_D1_OSR_2048 0x46
#define BARO_COMMAND_CONVERT_D1_OSR_4096 0x48
#define BARO_COMMAND_CONVERT_D2_OSR_256 0x50
#define BARO_COMMAND_CONVERT_D2_OSR_512 0x52
#define BARO_COMMAND_CONVERT_D2_OSR_1024 0x54
#define BARO_COMMAND_CONVERT_D2_OSR_2048 0x56
#define BARO_COMMAND_CONVERT_D2_OSR_4096 0x58
#define BARO_COMMAND_ADC_READ 0x00
#define BARO_COMMAND_BASE_PROM_READ 0xA0

/* PROM Addresses */
#define BARO_PROM_ADDRESS_C0 0x2
#define BARO_PROM_ADDRESS_C1 0x4
#define BARO_PROM_ADDRESS_C2 0x6
#define BARO_PROM_ADDRESS_C3 0x8
#define BARO_PROM_ADDRESS_C4 0xA
#define BARO_PROM_ADDRESS_C5 0xC

/* Conversion Times */
//#define BARO_CONVERSION_TIME_OSR_256 0.60
#define BARO_CONVERSION_TIME_OSR_256 1
#define BARO_CONVERSION_TIME_OSR_512 1.17
#define BARO_CONVERSION_TIME_OSR_1024 2.28
#define BARO_CONVERSION_TIME_OSR_2048 4.54
#define BARO_CONVERSION_TIME_OSR_4096 9.04

/* Extern */
extern I2C_HandleTypeDef hi2c1;
extern baro_data baro_data_to_mb;
extern osMutexId_t baro_mutex;

/* Macros */
#define READ_BARO_COEFF(IDX, command, coefficients) \
	command = BARO_COMMAND_BASE_PROM_READ | BARO_PROM_ADDRESS_C ## IDX; \
	HAL_I2C_Master_Transmit(&hi2c1, BARO_ADDRESS, &command, BARO_COMMAND_LENGTH, BARO_I2C_TIMEOUT); \
	HAL_I2C_Master_Receive(&hi2c1, BARO_ADDRESS, read_buffer, BARO_COEFFICIENT_LENGTH, BARO_I2C_TIMEOUT); \
	coefficients[IDX] = (read_buffer[0] << 8) | read_buffer[1];


#define CALC_TEMP_PRESS(DELAY, wait_time, command, pressure_raw, temperature_raw, temperature, pressure, coefficients) \
	wait_time = BARO_CONVERSION_TIME_OSR_ ## DELAY * osKernelGetTickFreq() / 1000; \
	command = BARO_COMMAND_CONVERT_D1_OSR_ ## DELAY; \
	HAL_I2C_Master_Transmit(&hi2c1, BARO_ADDRESS, &command, BARO_COMMAND_LENGTH, BARO_I2C_TIMEOUT); \
	osDelay(wait_time); \
	command = BARO_COMMAND_ADC_READ; \
	HAL_I2C_Master_Transmit(&hi2c1, BARO_ADDRESS, &command, BARO_COMMAND_LENGTH, BARO_I2C_TIMEOUT); \
	HAL_I2C_Master_Receive(&hi2c1, BARO_ADDRESS, read_buffer, BARO_DATA_LENGTH, BARO_I2C_TIMEOUT); \
	pressure_raw = (read_buffer[0] << 16) | (read_buffer[1] << 8) | read_buffer[2]; \
	command = BARO_COMMAND_CONVERT_D2_OSR_ ## DELAY; \
	HAL_I2C_Master_Transmit(&hi2c1, BARO_ADDRESS, &command,	BARO_COMMAND_LENGTH, BARO_I2C_TIMEOUT); \
	osDelay(wait_time); \
	command = BARO_COMMAND_ADC_READ; \
	HAL_I2C_Master_Transmit(&hi2c1, BARO_ADDRESS, &command,	BARO_COMMAND_LENGTH, BARO_I2C_TIMEOUT); \
	HAL_I2C_Master_Receive(&hi2c1, BARO_ADDRESS, read_buffer, BARO_DATA_LENGTH, BARO_I2C_TIMEOUT); \
	temperature_raw = (read_buffer[0] << 16) | (read_buffer[1] << 8) | read_buffer[2]; \
	vCalculateTempPressure(temperature, pressure, &temperature_raw,	&pressure_raw, coefficients);

/* Tasks */
void vTaskBaroRead(void *argument);

#endif /* INC_TASK_BARO_READ_H_ */
