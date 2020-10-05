/*
 * epos4.h
 *
 *  Created on: Jun 12, 2020
 *      Author: Jonas
 */

#ifndef INC_EPOS4_EPOS4_H_
#define INC_EPOS4_EPOS4_H_

#include "main.h"
#include "cmsis_os.h"

osStatus_t enable_motor();

osStatus_t disable_motor();

osStatus_t set_position_mode(int8_t position_mode);

osStatus_t move_to_position(int32_t position);

osStatus_t configure_ppm(int32_t velocity, int32_t acceleration,
                        int32_t deceleration);

osStatus_t get_position(int32_t *position);

/* external */
extern UART_HandleTypeDef huart4;

#endif /* INC_EPOS4_EPOS4_H_ */
