#include "drivers/gps/gps.h"
#include <string.h>

uint8_t _parse_data(struct gps_device *dev, uint8_t *data) {
	uint8_t crc = 0;
	for(int i = 0; i < 17; i++){
		crc += data[i];
	}
	if(crc == data[17]){
		dev->data.hour = data[0];
		dev->data.minute = data[1];
		dev->data.second = data[2];

		dev->data.lat_deg = data[3];
		dev->data.lat_decimal = ((uint32_t)data[4]<<24) + ((uint32_t)data[5]<<16) + ((uint32_t)data[6]<<8) + data[7];

		dev->data.lon_deg = data[8];
		dev->data.lon_decimal = ((uint32_t)data[9]<<24) + ((uint32_t)data[10]<<16) + ((uint32_t)data[11]<<8) + data[12];

		dev->data.fix = data[13];

		dev->data.satellite = data[14];

		dev->data.altitude = ((uint16_t)data[15]<<8) + data [16];
		return 1;
	}
	else return 0;
}

void _buffer_reset(struct gps_device *dev) {
	for (int i = 0; i < BUFFER_SIZE; i++){
		gps_data[dev->id][i] = 0;
	}
}

void gps_dma_init(struct gps_device *dev) {
  HAL_UART_DMAStop(dev->uart_bus);
  HAL_UART_Receive_DMA(dev->uart_bus, gps_data[dev->id], BUFFER_SIZE);
  HAL_UART_DMAResume(dev->uart_bus);
}

uint8_t gps_read_sensor(struct gps_device *dev) {
  if (gps_data[dev->id][17] != 0){
	  HAL_UART_DMAStop(dev->uart_bus);
	  _parse_data(dev,gps_data[dev->id]);
	  _buffer_reset(dev);
	  HAL_UART_Receive_DMA(dev->uart_bus, gps_data[dev->id], BUFFER_SIZE);
  } else {
	  if (dev->timeout_counter >= 5){
		  _buffer_reset(dev);
		  HAL_UART_Receive_DMA(dev->uart_bus, gps_data[dev->id], BUFFER_SIZE);
		  dev->timeout_counter = 0;
		  return 0;
	  }
	  else dev->timeout_counter++;
  }
  return 1;
}

void gps_reset(struct gps_device *dev) {
  dev->data.hour = 0;
  dev->data.minute = 0;
  dev->data.second = 0;

  dev->data.lat_deg = 0;
  dev->data.lat_decimal = 0;

  dev->data.lon_deg = 0;
  dev->data.lon_decimal = 0;

  dev->data.fix = 0;

  dev->data.satellite = 0;

  dev->data.HDOP = 0;

  dev->data.altitude = 0;
}
