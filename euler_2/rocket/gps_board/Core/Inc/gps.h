/*
 * gps.h
 *
 *  Created on: Sep 29, 2020
 *      Author: Luca
 */

#ifndef INC_DEVICES_GPS_H_
#define INC_DEVICES_GPS_H_

#include "stm32f1xx_hal.h"


#define BUFFER_SIZE 200



uint8_t nmea_message[BUFFER_SIZE];

// *** Macros *** //

// *** Defines *** //

// *** structs *** //


// *** Global Functions *** //
void _parse_nmea(int size);
extern void gps_character_received(uint8_t data);

extern UART_HandleTypeDef huart2;
#endif /* INC_DEVICES_GPS_H_ */
