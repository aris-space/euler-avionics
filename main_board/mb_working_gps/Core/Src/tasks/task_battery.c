/*
 * task_gps.c
 *
 *  Created on: Jun 26, 2020
 *      Author: Luca Jost
 */

#include "tasks/task_battery.h"

#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Util/util.h>


#define V25 1.43
#define AVG_SLOPE .0043

float get_temp (uint16_t adc_value){
	float VSENSE;
	VSENSE = 2.5/4096 * adc_value;
	return ((V25 - VSENSE) / AVG_SLOPE + 25);
}


void vTaskBattery(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;


	/* Initialise Variables */
	double mah;
	double curr = 0;
	double supp = 0;
	double bat = 0;

	uint16_t supply;
	uint16_t battery;
	uint16_t current;
	uint16_t consumption;
	uint16_t power;

	int counter = 0;

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / BATTERY_SAMPLE_RATE;

	//ADC init
	uint32_t adc_value[5];

	HAL_ADC_Stop_DMA(&hadc1);
	HAL_ADC_Start_DMA(&hadc1, adc_value, 5);

	osDelay(500);



	for (;;) {
		tick_count += tick_update;

		double current2 = ((double)adc_value[0] * (2.5/4096.0) - (3.3*0.107)) / 0.264; // CURR2
		float supply_voltage = adc_value[1] * (2.5/4096) * 2; // 3V3
		float battery_voltage = adc_value[2] * (2.5/4096) * 5.2; // BAT
		double current1 = ((double)adc_value[3] * (2.5/4096.0) - (3.3*0.107)) / 0.264; // CURR1
		get_temp(adc_value[4]); // temp

		// Filter adc values
		if (counter < 50) {
			counter++;
			curr += current1 + current2;
			supp += supply_voltage;
			bat += battery_voltage;
		} else {
			counter = 0;
			mah += (curr) / (BATTERY_SAMPLE_RATE * 3.6);
			consumption = (uint16_t)mah;
			current = (uint16_t)(curr*1000)/50;
			power = (curr*1000)/50 * (battery_voltage/50);
			supply = (uint16_t)(supp * 20);
			battery = (uint16_t)(bat * 20);
			curr = 0;
			bat = 0;
			supp = 0;
			UsbPrint("[BATTERY] Supply: %d Battery: %d Current: %dmA, Consumption: %dmAh Power: %dmW\n",
											supply, battery, current, consumption, power);
		}














		osDelayUntil(tick_count);
	}
}


