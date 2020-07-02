/*
 * task_gps.c
 *
 *  Created on: Jun 26, 2020
 *      Author: Luca Jost
 */

#include "tasks/task_gps.h"
#include "GPS/gps.h"

#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Util/util.h>


UBLOX GPS1 = {0,&huart1};
UBLOX GPS2 = {1,&huart2};
UBLOX GPS3 = {2,&huart3};

void vTaskGps(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;


	/* Initialise Variables */


	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / GPS_SAMPLE_RATE;
	osDelay(500);

	gps_dma_init(&GPS1);
	gps_dma_init(&GPS2);
	gps_dma_init(&GPS3);

	for (;;) {
		tick_count += tick_update;

		gps_read_sensor(&GPS1);
		gps_read_sensor(&GPS2);
		gps_read_sensor(&GPS3);


		UsbPrint("[GPS1] Time: %d:%d.%d Lat: %d.%ld Lon: %d.%ld Sats: %d, Alt: %d\n",
								GPS1.data.hour, GPS1.data.minute, GPS1.data.second, GPS1.data.lat_deg,
								GPS1.data.lat_decimal, GPS1.data.lon_deg, GPS1.data.lon_decimal, GPS1.data.satellite, GPS1.data.altitude);

		UsbPrint("[GPS2] Time: %d:%d.%d Lat: %d.%ld Lon: %d.%ld Sats: %d Alt: %d\n",
								GPS2.data.hour, GPS2.data.minute, GPS2.data.second, GPS2.data.lat_deg,
								GPS2.data.lat_decimal, GPS2.data.lon_deg, GPS2.data.lon_decimal, GPS2.data.satellite, GPS2.data.altitude);

		UsbPrint("[GPS3] Time: %d:%d.%d Lat: %d.%ld Lon: %d.%ld Sats: %d, Alt: %d\n",
								GPS3.data.hour, GPS3.data.minute, GPS3.data.second, GPS3.data.lat_deg,
								GPS3.data.lat_decimal, GPS3.data.lon_deg, GPS3.data.lon_decimal, GPS3.data.satellite, GPS3.data.altitude);




		osDelayUntil(tick_count);
	}
}

