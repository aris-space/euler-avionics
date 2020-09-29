// GPS Device Library
// Author: Luca Jost
// 26.06.2020

#ifndef _GPS
#define _GPS

#include "stm32h7xx_hal.h"

#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Util/util.h>

#define HEADER_SIZE 3
#define BUFFER_SIZE 1000
#define DATA_SIZE   70
#define GPS_NUMBER  3

uint8_t gps_data[GPS_NUMBER][BUFFER_SIZE];

uint8_t gps_gga[GPS_NUMBER][DATA_SIZE];

// *** Macros *** //

// *** Defines *** //

// *** structs *** //

typedef struct gps_device {
  uint8_t id;
  UART_HandleTypeDef *uart_bus;
  gps_data_t data;
} UBLOX;

// *** Global Functions *** //

extern uint8_t gps_read_sensor(struct gps_device *dev);
extern void gps_dma_init(struct gps_device *dev);
extern void gps_reset(struct gps_device *dev);

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

#endif
