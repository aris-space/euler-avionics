/*
 * gps.c
 *
 *  Created on: Sep 29, 2020
 *      Author: Luca
 */


#include "gps.h"

const uint8_t ubxgga[3] = {'G', 'G', 'A'};

void gps_character_received(uint8_t data){
	static int index = 0;
	static int i = 0;
	if (data == ubxgga[index] && index < 3) index++;
	else if (index >= 3 && data != '*' && i < BUFFER_SIZE) {
		nmea_message[i] = data;
		i++;
	} else if (index >= 3 && data == '*'){
		_parse_nmea(i);
		i = 0;
		index = 0;


	}
}

void to_char_array32 (uint8_t* buffer, uint32_t data){
	buffer[3] = data;
	buffer[2] = (data & 0xFF00) >> 8;
	buffer[1] = (data & 0xFF0000) >> 16;
	buffer[0] = (data & 0xFF000000) >> 24;
}

void to_char_array16 (uint8_t* buffer, uint16_t data){
	buffer[1] = data;
	buffer[0] = (data & 0xFF00) >> 8;
}


void _parse_nmea (int size) {
		uint8_t out_buff [18] = {0};
		for (int i = 0; i < size; i++) nmea_message[i] = nmea_message[i] - 48;
		if (size < 25) return;
		if (size >= 30){
			out_buff[0] = nmea_message[1] * 10 + nmea_message[2];	// Hour
			out_buff[1] = nmea_message[3] * 10 + nmea_message[4];	// Minute
			out_buff[2] = nmea_message[5] * 10 + nmea_message[6];	// Second
			if ((out_buff[0] > 24) || (out_buff[1] > 60) || (out_buff[2] > 60)) return;
		}
		if (size >= 50) {
			out_buff[3] = nmea_message[11] * 10 + nmea_message[12]; // lat
			uint32_t lat_decimal =
				(nmea_message[13] * 1e7 + nmea_message[14] * 1e6 + nmea_message[16] * 1e5 + nmea_message[17] * 1e4 +
						nmea_message[18] * 1e3 + nmea_message[19] * 1e2 + nmea_message[20]) / 6;
			to_char_array32(&out_buff[4], lat_decimal);

			out_buff[8] = nmea_message[25] * 10 + nmea_message[26]; // lon
			uint32_t lon_decimal =
				(nmea_message[27] * 1e7 + nmea_message[28] * 1e6 + nmea_message[30] * 1e5 + nmea_message[31] * 1e4 +
						nmea_message[32] * 1e3 + nmea_message[33] * 1e2 + nmea_message[34]) / 6;
			to_char_array32(&out_buff[9], lon_decimal);

			out_buff[13] = nmea_message[38]; // fix

			out_buff[14] = nmea_message[40] * 10 + nmea_message[41];	// sats

			uint16_t altitude = nmea_message[48] * 100 + nmea_message[49] * 10 + nmea_message[50];
			to_char_array16(&out_buff[15], altitude); // altitude


		}
		for(int i = 0; i < 17; i++){
			out_buff[17] += out_buff[i];		// Checksum
		}
		HAL_UART_Transmit(&huart2, out_buff, 18,200);
}



