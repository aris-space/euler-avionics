/*
 * flash_driver.h
 *
 *  Created on: Aug 20, 2020
 *      Author: Jonas
 */

#ifndef INC_FLASH_FLASH_DRIVER_H_
#define INC_FLASH_FLASH_DRIVER_H_

#include "main.h"
#include "cmsis_os.h"
#include <string.h>

extern SPI_HandleTypeDef hspi4;

typedef enum {
	flash_BUSY = 1, flash_IDLE
} flashStatus_t;

/* Functions */
int32_t read_chip_id();
flashStatus_t check_busy();
osStatus_t write_data(uint8_t* address, uint8_t* data, int32_t size);
osStatus_t write_read_command(uint8_t* write_data, uint8_t* read_data, int32_t size);

osStatus_t write_enable();
osStatus_t write_disable();

#endif /* INC_FLASH_FLASH_DRIVER_H_ */
