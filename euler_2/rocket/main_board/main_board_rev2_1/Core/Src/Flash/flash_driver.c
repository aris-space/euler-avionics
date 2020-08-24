/*
 * flash_driver.c
 *
 *  Created on: Aug 20, 2020
 *      Author: Jonas
 */

#include "Flash/flash_driver.h"

int32_t read_chip_id(){
	int32_t chip_id = 0;
	uint8_t buffer[4] = { 0 };
	uint8_t receive_buffer[4] = { 0 };
	buffer[0] = 0x90;

//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
//	HAL_SPI_Transmit_DMA(&hspi4, buffer, 4);
//	HAL_SPI_Receive_DMA(&hspi4, receive_buffer, 4);

	write_read_command(buffer, receive_buffer, sizeof(buffer));

	chip_id = receive_buffer[0] << 4 | receive_buffer[1];
	return chip_id;
}

flashStatus_t check_busy(){
	flashStatus_t status = flash_IDLE;
	uint8_t read_buffer = 0;
	/* check Status Register 1 = command 0x05 */
	uint8_t status_register_command = 0x05;
	write_read_command(&status_register_command, &read_buffer, 1);
	if(read_buffer << 7 == 0x80){
		status = flash_BUSY;
	}

	return status;
}

osStatus_t write_data(uint8_t* address, uint8_t* data, int32_t size){
	osStatus_t status = osOK;
	uint8_t write_command = 0x02;
	uint8_t write_buffer[4+size];
	uint8_t dummy_buffer[4+size];

	write_buffer[0] = write_command;
	memcpy(&write_buffer[1], address, 3);
	memcpy(&write_buffer[4], data, 3);

	write_enable();
	write_read_command(write_buffer, dummy_buffer, size);
	while(check_busy() == flash_BUSY){
		osDelay(1);
	}
	write_disable();

	return status;
}

osStatus_t read_data(uint8_t* address, uint8_t* data, int32_t size){
	osStatus_t status = osOK;
	uint8_t write_command = 0x03;
	uint8_t write_buffer[4+size];
	uint8_t dummy_buffer[4+size];

	write_buffer[0] = write_command;
	memcpy(&write_buffer[1], address, 3);
	memcpy(&write_buffer[4], data, 3);

	write_enable();
	write_read_command(write_buffer, dummy_buffer, size);
	while(check_busy() == flash_BUSY){
		osDelay(1);
	}
	write_disable();

	return status;
}


osStatus_t write_enable(){
	uint8_t enable_command = 0x06;
	uint8_t read_buffer = 0;
	return write_read_command(&enable_command, &read_buffer, 1);
}

osStatus_t write_disable(){
	uint8_t enable_command = 0x04;
	uint8_t read_buffer = 0;
	return write_read_command(&enable_command, &read_buffer, 1);
}


osStatus_t write_read_command(uint8_t* write_data, uint8_t* read_data, int32_t size){
	osStatus_t status = osOK;
	HAL_StatusTypeDef HAL_status = HAL_ERROR;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_status = HAL_SPI_Transmit_DMA(&hspi4, write_data, size);
	HAL_status = HAL_SPI_Receive_DMA(&hspi4, read_data, size);
	if (HAL_status != HAL_OK){
		status = osError;
	}

	return status;
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}
