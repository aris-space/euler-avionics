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

typedef enum { flash_BUSY = 1, flash_IDLE } flashStatus_t;

typedef struct {
  char s[5];
  uint32_t data[5];
} test_data_t;

/* Functions */
int32_t read_chip_id();
flashStatus_t check_busy();
osStatus_t write_data(uint32_t address, void const *data, uint16_t size);
osStatus_t write_command(uint8_t *write_data, uint16_t size);
osStatus_t read_data(uint32_t address, void *const data, uint16_t size);
osStatus_t write_read_command(uint8_t *write_data, uint8_t *read_data,
                              uint16_t size);
osStatus_t erase_4KB(uint32_t address);
osStatus_t erase_32KB(uint32_t address);
osStatus_t erase_64KB(uint32_t address);
osStatus_t write_enable();
osStatus_t write_disable();
osStatus_t erase_chip();

/* Registers */

#define WRITE_DATA_REGISTER       0x02
#define READ_DATA_REGISTER        0x03
#define WRITE_DISABLE_REGISTER    0x04
#define READ_STATUS_REGISTER_1    0x05
#define WRITE_ENABLE_REGISTER     0x06
#define FAST_READ_DATA_REGISTER   0x0B
#define SECTOR_ERASE_REGISTER     0x20
#define KB32_BLOCK_ERASE_REGISTER 0x52
#define KB64_BLOCK_ERASE_REGISTER 0xD8
#define CHIP_ERASE_REGISTER       0x60
#define DEVICE_ID_REGISTER        0x90
#define READ_STATUS_REGISTER_2    0x35
#define READ_STATUS_REGISTER_3    0x15

#endif /* INC_FLASH_FLASH_DRIVER_H_ */
