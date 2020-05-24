/*
 * task_motor_control.c
 *
 *  Created on: May 23, 2020
 *      Author: Jonas
 */

#include "tasks/task_motor_control.h"

uint16_t calculateCRC(uint8_t *data, uint8_t len);

uint8_t rx_data[10] = { 0 };

void vTaskMotorCont(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / MOTOR_TASK_FREQUENCY;
	uint8_t counter = 10;

	for (;;) {
		tick_count += tick_update;

		/* Read Command */
		uint8_t byte_stream_read[10] = { 0 };
		byte_stream_read[0] = 0x90;		// DLE
		byte_stream_read[1] = 0x02;		// STX
		byte_stream_read[2] = 0x60;		// Read Object
		byte_stream_read[3] = 0x02;		// Length of stuff sent
		byte_stream_read[4] = 0x01;		// Node ID
		byte_stream_read[5] = 0x7A;		// Index Low Byte
		byte_stream_read[6] = 0x60;		// Index High byte
		byte_stream_read[7] = 0x00;		// Subindex of object

		/* CRC data array */
		uint8_t crc_data_array[6] = { 0 };
		memcpy(crc_data_array, &byte_stream_read[2], 6*sizeof(*byte_stream_read));

		uint16_t crc_calc = 0;
		crc_calc = calculateCRC(crc_data_array, 6);

		byte_stream_read[8] = crc_calc & 0xFF;;		// CRC low byte
		byte_stream_read[9] = (crc_calc >> 8) & 0xFF;;		// CRC high byte
		byte_stream_read[8] = 0x6C;		// CRC low byte
		byte_stream_read[9] = 0xE6;		// CRC high byte


		/* rx buffer */
		HAL_StatusTypeDef status;
		status = HAL_UART_Transmit(&huart7, byte_stream_read, 10, 10);
		HAL_UART_Receive_IT(&huart7, rx_data, 10);

		uint8_t byte_stream_write[14] = { 0 };

		if (counter > 20){
			counter = 10;
			byte_stream_write[12] = 0x41;		// CRC low byte
			byte_stream_write[13] = 0xBF;		// CRC high byte
		}
		else{
			byte_stream_write[12] = 0x87;		// CRC low byte
			byte_stream_write[13] = 0x39;		// CRC high byte
			counter = 30;
		}
		/* Write Command */

		byte_stream_write[0] = 0x90;		// DLE
		byte_stream_write[1] = 0x02;		// STX
		byte_stream_write[2] = 0x68;		// Write Object
		byte_stream_write[3] = 0x04;		// Length of stuff sent
		byte_stream_write[4] = 0x01;		// Node ID
		byte_stream_write[5] = 0x7A;		// Index Low Byte
		byte_stream_write[6] = 0x60;		// Index High byte
		byte_stream_write[7] = 0x00;		// Subindex of object
		byte_stream_write[8] = counter;		// Data LSB
		byte_stream_write[9] = 0x00;		//
		byte_stream_write[10] = 0x00;		//
		byte_stream_write[11] = 0x00;		// Data MSB
//		byte_stream_write[12] = 0x41;		// CRC low byte
//		byte_stream_write[13] = 0xBF;		// CRC high byte
		status = HAL_UART_Transmit(&huart7, byte_stream_write, 14, 10);
		osDelay(1);

		byte_stream_write[0] = 0x90;		// DLE
		byte_stream_write[1] = 0x02;		// STX
		byte_stream_write[2] = 0x68;		// Write Object
		byte_stream_write[3] = 0x04;		// Length of stuff sent
		byte_stream_write[4] = 0x01;		// Node ID
		byte_stream_write[5] = 0x40;		// Index Low Byte
		byte_stream_write[6] = 0x60;		// Index High byte
		byte_stream_write[7] = 0x00;		// Subindex of object
		byte_stream_write[8] = 0x3F;		// Data LSB
		byte_stream_write[9] = 0x00;		//
		byte_stream_write[10] = 0x00;		//
		byte_stream_write[11] = 0x00;		// Data MSB
		byte_stream_write[12] = 0x16;		// CRC low byte
		byte_stream_write[13] = 0xC2;		// CRC high byte
		status = HAL_UART_Transmit(&huart7, byte_stream_write, 14, 10);
		osDelay(1);

		byte_stream_write[0] = 0x90;		// DLE
		byte_stream_write[1] = 0x02;		// STX
		byte_stream_write[2] = 0x68;		// Write Object
		byte_stream_write[3] = 0x04;		// Length of stuff sent
		byte_stream_write[4] = 0x01;		// Node ID
		byte_stream_write[5] = 0x40;		// Index Low Byte
		byte_stream_write[6] = 0x60;		// Index High byte
		byte_stream_write[7] = 0x00;		// Subindex of object
		byte_stream_write[8] = 0x0F;		// Data LSB
		byte_stream_write[9] = 0x00;		//
		byte_stream_write[10] = 0x00;		//
		byte_stream_write[11] = 0x00;		// Data MSB
		byte_stream_write[12] = 0xB3;		// CRC low byte
		byte_stream_write[13] = 0x07;		// CRC high byte
		status = HAL_UART_Transmit(&huart7, byte_stream_write, 14, 10);


		osDelayUntil(tick_count);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	HAL_UART_Receive_IT(&huart7, rx_data, sizeof(rx_data));
}


uint16_t calculateCRC(uint8_t *data, uint8_t len) {
  uint16_t shifter, c;
  uint16_t carry;
  uint16_t crcCalc = 0;

  for (int i = 0; i < len + 2; i += 2) {
    shifter = 0x8000;
    if (i == 0) {
      c = data[i] << 8 | data[i+1];
    } else if (i == len) {
      c = 0;
    } else {
      c = data[i+1] << 8 | data[i];
    }
    do {
      carry = crcCalc & 0x8000;
      crcCalc <<= 1;
      if(c & shifter) crcCalc++;
      if(carry) crcCalc ^= 0x1021;
      shifter >>= 1;
    } while(shifter);
  }
  return crcCalc;
}

