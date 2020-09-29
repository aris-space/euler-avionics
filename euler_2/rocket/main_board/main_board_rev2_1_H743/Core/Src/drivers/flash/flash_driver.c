/*
 * flash_driver.c
 *
 *  Created on: Aug 20, 2020
 *      Author: Jonas
 */

#include "drivers/flash/flash_driver.h"

int32_t read_chip_id() {
  int32_t chip_id = 0;
  uint8_t buffer[4] = {0};
  uint8_t receive_buffer[4] = {0};
  buffer[0] = DEVICE_ID_REGISTER;

  //	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  //	HAL_SPI_Transmit_DMA(&hspi4, buffer, 4);
  //	HAL_SPI_Receive_DMA(&hspi4, receive_buffer, 4);

  write_read_command(buffer, receive_buffer, sizeof(buffer));

  chip_id = receive_buffer[0] << 4 | receive_buffer[1];
  return chip_id;
}

flashStatus_t check_busy() {
  flashStatus_t status = flash_IDLE;
  uint8_t read_buffer = 0;
  /* check Status Register 1*/
  uint8_t status_register_command = READ_STATUS_REGISTER_3;
  write_read_command(&status_register_command, &read_buffer, 1);
  if (read_buffer << 7 == 0x80) {
    status = flash_BUSY;
  }

  return status;
}

osStatus_t write_data(uint32_t address, void const *data, uint16_t size) {
  osStatus_t status = osOK;

  /* first get data in local uint8_t buffer */
  uint8_t data_buffer[size];
  memcpy(&data_buffer[0], data, size);
  uint8_t block_size = 60;

  /* get the number of needed writes */
  uint32_t number_writes = size / block_size;

  uint8_t write_buffer[4 + block_size];

  /* write all full buffers */

  for (int i = 0; i < number_writes; i++) {
    write_buffer[0] = WRITE_DATA_REGISTER;
    write_buffer[1] = (address + block_size * i) >> 16;
    write_buffer[2] = (address + block_size * i) >> 8;
    write_buffer[3] = (address + block_size * i) >> 0;
    memcpy(&write_buffer[4], &data_buffer[block_size * i], block_size);

    write_enable();
    osDelay(1);
    write_command(write_buffer, sizeof(write_buffer));
  }
  /* get how many bytes are left to write */

  uint32_t single_bytes = size % block_size;
  uint8_t single_write_buffer[4 + single_bytes];

  /* write last bit of bytes */

  single_write_buffer[0] = WRITE_DATA_REGISTER;
  single_write_buffer[1] = (address + block_size * number_writes) >> 16;
  single_write_buffer[2] = (address + block_size * number_writes) >> 8;
  single_write_buffer[3] = (address + block_size * number_writes) >> 0;

  memcpy(&single_write_buffer[4], &data_buffer[block_size * number_writes],
         single_bytes);

  write_enable();
  osDelay(1);
  write_command(single_write_buffer, sizeof(single_write_buffer));

  return status;
}

osStatus_t read_data(uint32_t address, void *const data, uint16_t size) {
  osStatus_t status = osOK;
  uint8_t write_buffer[4];

  write_buffer[0] = READ_DATA_REGISTER;
  write_buffer[1] = (address) >> 16;
  write_buffer[2] = (address) >> 8;
  write_buffer[3] = (address) >> 0;

  uint8_t read_buffer[size];

  write_read_command(write_buffer, read_buffer, size);

  memcpy(data, read_buffer, size);

  return status;
}

osStatus_t erase_4KB(uint32_t address) {
  osStatus_t status = osOK;
  uint8_t write_buffer[4];

  write_buffer[0] = SECTOR_ERASE_REGISTER;
  write_buffer[1] = (address) >> 16;
  write_buffer[2] = (address) >> 8;
  write_buffer[3] = (address) >> 0;

  return write_command(write_buffer, sizeof(write_buffer));
}

osStatus_t erase_32KB(uint32_t address) {
  osStatus_t status = osOK;
  uint8_t write_buffer[4];

  write_buffer[0] = KB32_BLOCK_ERASE_REGISTER;
  write_buffer[1] = (address) >> 16;
  write_buffer[2] = (address) >> 8;
  write_buffer[3] = (address) >> 0;

  return write_command(write_buffer, sizeof(write_buffer));
}

osStatus_t erase_64KB(uint32_t address) {
  osStatus_t status = osOK;
  uint8_t write_buffer[4];

  write_buffer[0] = KB64_BLOCK_ERASE_REGISTER;
  write_buffer[1] = (address) >> 16;
  write_buffer[2] = (address) >> 8;
  write_buffer[3] = (address) >> 0;

  return write_command(write_buffer, sizeof(write_buffer));
}

osStatus_t write_enable() {
  uint8_t enable_command = WRITE_ENABLE_REGISTER;
  return write_command(&enable_command, sizeof(enable_command));
}

osStatus_t write_disable() {
  uint8_t disable_command = WRITE_DISABLE_REGISTER;
  return write_command(&disable_command, sizeof(disable_command));
}

osStatus_t erase_chip() {
  uint8_t erase_chip_command = CHIP_ERASE_REGISTER;
  write_enable();
  return write_command(&erase_chip_command, sizeof(erase_chip_command));
}

osStatus_t write_read_command(uint8_t *write_data, uint8_t *read_data,
                              uint16_t size) {
  osStatus_t status = osOK;
  HAL_StatusTypeDef HAL_status = HAL_ERROR;

  //	while(check_busy() == flash_BUSY){
  //		osDelay(1);
  //	}

  //	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  //	HAL_status = HAL_SPI_Transmit_DMA(&hspi4, write_data, size);
  //	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  //	HAL_status = HAL_SPI_Receive_DMA(&hspi4, read_data, size);
  HAL_GPIO_WritePin(SPI4_CS_GPIO_Port, SPI4_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi4, write_data, size, 5);
  HAL_SPI_Receive(&hspi4, read_data, size, 5);
  HAL_GPIO_WritePin(SPI4_CS_GPIO_Port, SPI4_CS_Pin, GPIO_PIN_SET);

  if (HAL_status != HAL_OK) {
    status = osError;
  }

  return status;
}

osStatus_t write_command(uint8_t *write_data, uint16_t size) {
  osStatus_t status = osOK;
  HAL_StatusTypeDef HAL_status = HAL_ERROR;

  //	while(check_busy() == flash_BUSY){
  //		osDelay(1);
  //	}

  //	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  //	HAL_status = HAL_SPI_Transmit_DMA(&hspi4, write_data, size);
  HAL_GPIO_WritePin(SPI4_CS_GPIO_Port, SPI4_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi4, write_data, size, 10);
  HAL_GPIO_WritePin(SPI4_CS_GPIO_Port, SPI4_CS_Pin, GPIO_PIN_SET);
  //	HAL_SPIEx_FlushRxFifo(&hspi4);
  if (HAL_status != HAL_OK) {
    status = osError;
  }

  return status;
}

// void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
////	HAL_SPIEx_FlushRxFifo(hspi);
//}
// void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
//}
