// GPS Device Library
// Author: Luca Jost
// 26.06.2020

#ifndef _GPS
#define _GPS

#include <util/util.h>
#include "stm32h7xx_hal.h"

#define BUFFER_SIZE 18
#define GPS_NUMBER 3

uint8_t gps_data[GPS_NUMBER][BUFFER_SIZE];

// *** Macros *** //

// *** Defines *** //

// *** structs *** //

typedef struct gps_device {
  uint8_t id;
  UART_HandleTypeDef *uart_bus;
  uint8_t timeout_counter;
  gps_data_t data;
} UBLOX;

// *** Global Functions *** //

uint8_t gps_read_sensor(struct gps_device *dev);
void gps_dma_init(struct gps_device *dev);
void gps_reset(struct gps_device *dev);

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

#endif
