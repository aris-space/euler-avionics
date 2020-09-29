/*
 * epos4.c
 *
 *  Created on: Jun 12, 2020
 *      Author: Jonas
 */
#include "EPOS4/epos4.h"

uint16_t calculateCRC(uint8_t *data, uint8_t len) {
  uint16_t shifter, c;
  uint16_t carry;
  uint16_t crc = 0;

  for (int i = 0; i < len + 2; i += 2) {
    shifter = 0x8000;
    if (i == len) {
      c = 0;
    } else {
      c = data[i + 1] << 8 | data[i];
    }
    do {
      carry = crc & 0x8000;
      crc <<= 1;
      if (c & shifter) crc++;
      if (carry) crc ^= 0x1021;
      shifter >>= 1;
    } while (shifter);
  }
  return crc;
}

osStatus_t EnableMotor() {
  osStatus_t status = osError;

  uint8_t command[2];
  uint8_t data[4];
  uint8_t rx_buffer_write[20];

  /* Register for Motor Control */
  command[0] = 0x60;
  command[1] = 0x40;

  data[0] = 0x00;
  data[1] = 0x00;
  data[2] = 0x00;
  data[3] = 0x06;

  status = WriteCommand(command, data, rx_buffer_write);

  /* Register for Motor Control */
  command[0] = 0x60;
  command[1] = 0x40;

  /* Fully Enable Controller */
  data[0] = 0x00;
  data[1] = 0x00;
  data[2] = 0x00;
  data[3] = 0x0F;

  status = WriteCommand(command, data, rx_buffer_write);

  /* Check if Motor is enabled */
  uint8_t rx_buffer_read[20];

  command[0] = 0x60;
  command[1] = 0x41;

  status = ReadCommand(command, rx_buffer_read);

  if (rx_buffer_read[8] == 0x37 && rx_buffer_read[9] == 0x04) {
    status = osOK;
  }

  return status;
}

osStatus_t DisableMotor() {
  osStatus_t status = osError;

  uint8_t command[2];
  uint8_t data[4];
  uint8_t rx_buffer_write[20];

  /* Register for Motor Control */
  command[0] = 0x60;
  command[1] = 0x41;

  data[0] = 0x00;
  data[1] = 0x00;
  data[2] = 0x00;
  data[3] = 0x40;

  status = WriteCommand(command, data, rx_buffer_write);

  return status;
}

osStatus_t SetPositionMode(int8_t position_mode) {
  osStatus_t status = osError;

  uint8_t command[2];
  uint8_t data[4];
  uint8_t rx_buffer_write[20];

  /* Position Mode Register */
  command[0] = 0x60;
  command[1] = 0x60;

  /* Enable Cyclic Sync Position Mode */
  data[0] = 0x00;
  data[1] = 0x00;
  data[2] = 0x00;
  data[3] = position_mode;

  status = WriteCommand(command, data, rx_buffer_write);

  return status;
}

osStatus_t MoveToPosition(int32_t position) {
  osStatus_t status = osError;

  uint8_t command[2];
  uint8_t data[4];
  uint8_t rx_buffer_write[20];

  /* Register for Desired Position */
  command[0] = 0x60;
  command[1] = 0x7A;

  data[0] = (position >> 24) & 0xFF;
  data[1] = (position >> 16) & 0xFF;
  data[2] = (position >> 8) & 0xFF;
  data[3] = position & 0xFF;

  status = WriteCommand(command, data, rx_buffer_write);

  return status;
}

osStatus_t MoveToPositionPPM(int32_t position) {
  osStatus_t status = osError;

  uint8_t command[2];
  uint8_t data[4];
  uint8_t rx_buffer_write[20];

  /* Write Desired Position */
  command[0] = 0x60;
  command[1] = 0x7A;

  data[0] = (position >> 24) & 0xFF;
  data[1] = (position >> 16) & 0xFF;
  data[2] = (position >> 8) & 0xFF;
  data[3] = position & 0xFF;

  status = WriteCommand(command, data, rx_buffer_write);

  /* Goto Position */
  command[0] = 0x60;
  command[1] = 0x40;

  data[0] = 0x00;
  data[1] = 0x00;
  data[2] = 0x00;
  data[3] = 0x3F;

  status = WriteCommand(command, data, rx_buffer_write);

  /* Disabe Movement Again */
  command[0] = 0x60;
  command[1] = 0x40;

  data[0] = 0x00;
  data[1] = 0x00;
  data[2] = 0x00;
  data[3] = 0x0F;

  status = WriteCommand(command, data, rx_buffer_write);

  return status;
}

osStatus_t ConfigurePPM(int32_t velocity, int32_t acceleration,
                        int32_t deceleration) {
  osStatus_t status = osError;

  uint8_t command[2];
  uint8_t data[4];
  uint8_t rx_buffer_write[20];

  /* Configure desired Velocity to be reached during Position Change */
  command[0] = 0x60;
  command[1] = 0x81;

  data[0] = (velocity >> 24) & 0xFF;
  data[1] = (velocity >> 16) & 0xFF;
  data[2] = (velocity >> 8) & 0xFF;
  data[3] = velocity & 0xFF;

  status = WriteCommand(command, data, rx_buffer_write);

  /* Configure desired acceleration to be reached during Position Change */
  command[0] = 0x60;
  command[1] = 0x83;

  data[0] = (acceleration >> 24) & 0xFF;
  data[1] = (acceleration >> 16) & 0xFF;
  data[2] = (acceleration >> 8) & 0xFF;
  data[3] = acceleration & 0xFF;

  status = WriteCommand(command, data, rx_buffer_write);

  /* Configure desired acceleration to be reached during Position Change */
  command[0] = 0x60;
  command[1] = 0x84;

  data[0] = (deceleration >> 24) & 0xFF;
  data[1] = (deceleration >> 16) & 0xFF;
  data[2] = (deceleration >> 8) & 0xFF;
  data[3] = deceleration & 0xFF;

  status = WriteCommand(command, data, rx_buffer_write);

  return status;
}

osStatus_t GetPosition(int32_t *position) {
  osStatus_t status = osError;

  uint8_t command[2];
  uint8_t rx_buffer_read[20];

  /* Register where the Current Position is written */
  command[0] = 0x60;
  command[1] = 0x64;

  status = ReadCommand(command, rx_buffer_read);

  *position = rx_buffer_read[8] + (rx_buffer_read[9] << 8) +
              (rx_buffer_read[10] << 16) + (rx_buffer_read[11] << 24);

  return status;
}

osStatus_t WriteCommand(uint8_t *command, uint8_t *data, uint8_t *rx_buffer) {
  osStatus_t status = osError;

  uint8_t byte_stream_write[14] = {0};

  uint8_t dma_buffer[10] = {0};

  byte_stream_write[0] = 0x90;        // DLE
  byte_stream_write[1] = 0x02;        // STX
  byte_stream_write[2] = 0x68;        // Write Object
  byte_stream_write[3] = 0x04;        // Length of Data in Words
  byte_stream_write[4] = 0x01;        // Node ID
  byte_stream_write[5] = command[1];  // Index Low Byte
  byte_stream_write[6] = command[0];  // Index High byte
  byte_stream_write[7] = 0x00;        // Subindex of object
  byte_stream_write[8] = data[3];     // Data - low byte
  byte_stream_write[9] = data[2];     // Data
  byte_stream_write[10] = data[1];    // Data
  byte_stream_write[11] = data[0];    // Data - high byte

  /* CRC Calculation */
  uint8_t crc_data_array[10] = {0};
  memcpy(crc_data_array, &byte_stream_write[2],
         10 * sizeof(*byte_stream_write));

  uint16_t crc_calc = 0;
  crc_calc = calculateCRC(crc_data_array, 10);

  byte_stream_write[12] = crc_calc & 0xFF;
  ;  // CRC low byte
  byte_stream_write[13] = (crc_calc >> 8) & 0xFF;
  ;  // CRC high byte

  HAL_UART_DMAResume(&huart4);
  HAL_UART_Transmit_DMA(&huart4, byte_stream_write, 14);

  HAL_UART_Receive_DMA(&huart4, dma_buffer, 10);
  osDelay(5);
  HAL_UART_DMAStop(&huart4);
  memcpy(rx_buffer, dma_buffer, 10);


  /* Check if we have an error code */
  if ((rx_buffer[7] | rx_buffer[6] | rx_buffer[5] | rx_buffer[4]) == 0) {
    status = osOK;
  }

  return status;
}

osStatus_t ReadCommand(uint8_t *command, uint8_t *rx_buffer) {
  osStatus_t status = osError;

  uint8_t byte_stream_read[10];

  uint8_t dma_buffer[14] = {0};

  byte_stream_read[0] = 0x90;        // DLE
  byte_stream_read[1] = 0x02;        // STX
  byte_stream_read[2] = 0x60;        // Read Object
  byte_stream_read[3] = 0x02;        // Length of stuff sent
  byte_stream_read[4] = 0x01;        // Node ID
  byte_stream_read[5] = command[1];  // Index Low Byte
  byte_stream_read[6] = command[0];  // Index High byte
  byte_stream_read[7] = 0x00;        // Subindex of object

  /* CRC data array */
  uint8_t crc_data_array[6] = {0};
  memcpy(crc_data_array, &byte_stream_read[2], 6 * sizeof(*byte_stream_read));

  uint16_t crc_calc = 0;
  crc_calc = calculateCRC(crc_data_array, 6);

  byte_stream_read[8] = crc_calc & 0xFF;
  ;  // CRC low byte
  byte_stream_read[9] = (crc_calc >> 8) & 0xFF;
  ;  // CRC high byte

  HAL_UART_DMAResume(&huart4);
  HAL_UART_Transmit_DMA(&huart4, byte_stream_read, 10);

  HAL_UART_Receive_DMA(&huart4, dma_buffer, 14);
  osDelay(5);
  HAL_UART_DMAStop(&huart4);
  memcpy(rx_buffer, dma_buffer, 14);

  /* check if we have an error code */
  if ((rx_buffer[7] | rx_buffer[6] | rx_buffer[5] | rx_buffer[4]) == 0) {
    status = osOK;
  }
  return status;
}
