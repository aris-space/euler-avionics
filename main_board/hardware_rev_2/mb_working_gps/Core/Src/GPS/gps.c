
#include "GPS/gps.h"

const uint8_t ubxgga[HEADER_SIZE] = {'G','G','A'};

uint32_t _gps_search_start(uint8_t *arr){
	for(int i=3; i<BUFFER_SIZE-DATA_SIZE; i++)
			{
			  if(arr[i-2] == ubxgga[0] && arr[i-1] == ubxgga[1] && arr[i] == ubxgga[2])
			    return i+1;
			}
	return 0;
}

uint8_t _parse_data (struct gps_device *dev, uint8_t* data){


	if (data[22] == 'N' || data[22] == 'S'){

		// ASCII to decimal conversion
		for(int i = 0; i < DATA_SIZE; i++) data[i] = data[i]-48;


		dev->data.hour = data[1]*10 + data[2];
		dev->data.minute = data[3]*10 + data[4];
		dev->data.second = data[5]*10 + data[6];

		dev->data.lat_deg = data[11]*10 +data[12];
		dev->data.lat_decimal = (data[13]*1e7 + data[14]*1e6 + data[16]*1e5 + data[17]*1e4 + data[18]*1e3 + data[19]*1e2 + data[20])/6;

		dev->data.lon_deg = data[25]*10 +data[26];
		dev->data.lon_decimal = (data[27]*1e7 + data[28]*1e6 + data[30]*1e5 + data[31]*1e4 + data[32]*1e3 + data[33]*1e2 + data[34])/6;

		dev->data.fix = data[38];

		dev->data.satellite = data[40]*10 + data[41];

		dev->data.HDOP = data[43]*100+data[45]*10+data[46];

		dev->data.altitude= data[48]*100+data[49]*10+data[50];

	}
	else {
		dev->data.fix = 0;
		return 0;
	}
	return 1;
}


void gps_dma_init(struct gps_device *dev){
	HAL_UART_DMAStop(dev->uart_bus);

	HAL_UART_Receive_DMA(dev->uart_bus, gps_data[dev->id], BUFFER_SIZE);

	HAL_UART_DMAResume(dev->uart_bus);
}



uint8_t gps_read_sensor (struct gps_device *dev){
	HAL_UART_DMAPause(dev->uart_bus);

	uint32_t data_start = _gps_search_start(gps_data[dev->id]);
	if (data_start > 0) memcpy(gps_gga[dev->id], gps_data[dev->id] + data_start, DATA_SIZE);

	HAL_UART_DMAResume(dev->uart_bus);

	return _parse_data(dev, gps_gga[dev->id]);
}
