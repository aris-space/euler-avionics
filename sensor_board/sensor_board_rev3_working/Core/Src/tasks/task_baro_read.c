/*
 * task_baro_read.c
 *
 *  Created on: Nov 1, 2019
 *      Author: Jonas
 */

#include "tasks/task_baro_read.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

void vInitBaroRead(uint16_t coefficients[]);
void vReadBaro(int32_t *temperature, int32_t *pressure, uint16_t coefficients[]);
void vCalculateTempPressure(int32_t *temperature, int32_t *pressure,
		uint32_t *temperature_raw, uint32_t *pressure_raw,
		uint16_t coefficients[]);

/**
 * @brief Function implementing the task_baro_read thread.
 * @param argument: Not used
 * @retval None
 */
void vTaskBaroRead(void *argument) {
	/* For periodic update */
	uint32_t tick_count, tick_update;
	/* actual measurements from sensor */
	int32_t temperature;
	int32_t pressure;

	/* needed for calculation of temperature and pressure. Is read out in initialization */
	uint16_t coefficients[6];
	vInitBaroRead(coefficients);

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / BARO_SAMPLING_FREQ;
	while (1) {
		tick_count += tick_update;
		vReadBaro(&temperature, &pressure, coefficients);

//		UsbPrint("P: %ld; T: %ld; t: %ld\n", pressure,
//				temperature, tick_count);

		/* If the Mutex is acquired we write the data into the right variable */
		if (osMutexAcquire(baro_mutex, BARO_MUTEX_TIMEOUT) == osOK) {
			baro_data_to_mb.temperature = temperature;
			baro_data_to_mb.pressure = pressure;
			baro_data_to_mb.ts = tick_count;
			osMutexRelease(baro_mutex);
		}

		osDelayUntil(tick_count);
	}
}

void vInitBaroRead(uint16_t coefficients[]) {
	uint8_t command;
	uint8_t read_buffer[2];
	uint32_t wait_reset_time;
	wait_reset_time = 3 * osKernelGetTickFreq() / 1000;

	/* Reset */
	command = BARO_COMMAND_RESET;
	HAL_I2C_Master_Transmit(&hi2c1, BARO_ADDRESS, &command, BARO_COMMAND_LENGTH,
	BARO_I2C_TIMEOUT);
	osDelay(wait_reset_time);

	/* PROM Read (Coefficients) */
	READ_BARO_COEFF(0, command, coefficients);

	READ_BARO_COEFF(1, command, coefficients);

	READ_BARO_COEFF(2, command, coefficients);

	READ_BARO_COEFF(3, command, coefficients);

	READ_BARO_COEFF(4, command, coefficients);

	READ_BARO_COEFF(5, command, coefficients);
}

void vReadBaro(int32_t *temperature, int32_t *pressure, uint16_t coefficients[]) {
	uint8_t command;
	uint32_t temperature_raw;
	uint32_t pressure_raw;
	uint8_t read_buffer[3];
	uint32_t wait_time_OSR;

	switch (BARO_OSR) {
	case 0:
		CALC_TEMP_PRESS(256, wait_time_OSR, command, pressure_raw,
				temperature_raw, temperature, pressure, coefficients)
		;
		break;
	case 1:
		CALC_TEMP_PRESS(512, wait_time_OSR, command, pressure_raw,
				temperature_raw, temperature, pressure, coefficients)
		;
		break;
	case 2:
		CALC_TEMP_PRESS(1024, wait_time_OSR, command, pressure_raw,
				temperature_raw, temperature, pressure, coefficients)
		;
		break;
	case 3:
		CALC_TEMP_PRESS(2048, wait_time_OSR, command, pressure_raw,
				temperature_raw, temperature, pressure, coefficients)
		;
		break;
	case 4:
		CALC_TEMP_PRESS(4096, wait_time_OSR, command, pressure_raw,
				temperature_raw, temperature, pressure, coefficients)
		;
		break;
	default:
		/* defaults to OSR 256 */
		CALC_TEMP_PRESS(256, wait_time_OSR, command, pressure_raw,
				temperature_raw, temperature, pressure, coefficients)
		;
	}
}

/* this is how we have to calculate temperature and Pressure which is written in the Datasheet */
void vCalculateTempPressure(int32_t *temperature, int32_t *pressure,
		uint32_t *temperature_raw, uint32_t *pressure_raw,
		uint16_t coefficients[]) {
	int64_t dT;
	int64_t OFF, SENS;

	dT = *temperature_raw - ((uint32_t) coefficients[4] << 8);
	/* Temperature in 2000  = 20.00° C */
	*temperature = (int32_t) 2000 + (dT * coefficients[5] >> 23);

	OFF = ((int64_t) coefficients[1] << 17) + ((coefficients[3] * dT) >> 6);
	SENS = ((int64_t) coefficients[0] << 16) + ((coefficients[2] * dT) >> 7);
	/* Pressure in 110002 = 1100.02 mbar */
	*pressure = (int32_t) ((((*pressure_raw * SENS) >> 21) - OFF) >> 15);
}
