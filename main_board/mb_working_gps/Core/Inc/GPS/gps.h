// GPS Device Library
// Author: Luca Jost
// 26.06.2020


#ifndef _GPS
#define _GPS

#include "stm32f7xx_hal.h"

#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Util/util.h>



#define HEADER_SIZE 3
#define BUFFER_SIZE 1000
#define DATA_SIZE 70


uint8_t gps_data [3][BUFFER_SIZE];

uint8_t gps_gga [3][DATA_SIZE];

// *** Macros *** //

// *** Defines *** //



// *** structs *** //

struct gps_data {
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
	uint8_t lat_deg;
	uint32_t lat_decimal;
	uint8_t lon_deg;
	uint32_t lon_decimal;
	uint8_t fix;
	uint8_t satellite;
	uint16_t HDOP;
	uint16_t altitude;
    };

typedef struct gps_device {
	uint8_t id;
	UART_HandleTypeDef* uart_bus;
	struct gps_data data;
    } UBLOX;


// *** Global Functions *** //

extern uint8_t gps_read_sensor (struct gps_device *dev);
extern void gps_dma_init(struct gps_device *dev);


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

#endif
