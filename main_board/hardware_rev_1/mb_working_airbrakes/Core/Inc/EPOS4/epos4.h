/*
 * epos4.h
 *
 *  Created on: Jun 12, 2020
 *      Author: Jonas
 */

#ifndef INC_EPOS4_EPOS4_H_
#define INC_EPOS4_EPOS4_H_

#include "cmsis_os.h"
#include "main.h"
#include <string.h>


uint16_t calculateCRC(uint8_t *data, uint8_t len);
osStatus_t EnableMotor();
osStatus_t DisableMotor();
osStatus_t SetPositionMode(int8_t position_mode);
osStatus_t MoveToPosition(int32_t position);
osStatus_t MoveToPositionPPM(int32_t position);
osStatus_t ConfigurePPM(int32_t velocity, int32_t acceleration, int32_t deceleration);
osStatus_t GetPosition(int32_t *position);
osStatus_t WriteCommand(uint8_t *command, uint8_t *data, uint8_t *rx_buffer);
osStatus_t ReadCommand(uint8_t *command, uint8_t *rx_buffer);

/* external */
extern UART_HandleTypeDef huart7;




#endif /* INC_EPOS4_EPOS4_H_ */
