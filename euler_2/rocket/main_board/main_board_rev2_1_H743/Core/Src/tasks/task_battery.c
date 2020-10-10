/*
 * task_gps.c
 *
 *  Created on: Jun 26, 2020
 *      Author: Luca Jost
 */

#include "util/logging_util.h"
#include "tasks/task_battery.h"

/* DMA Buffer */
uint32_t adc_value[4];

/* Buffers for running avarrage */
double current_buffer[50];
double supply_buffer[50];
double battery_buffer[50];

void vTaskBattery(void *argument) {
  /* For periodic update */
  uint32_t tick_count, tick_update;

  /* Initialise Variables */
  double mah = 0;
  double curr = 0;
  double supp = 0;
  double bat = 0;

  battery_data_t battery_data = {0};

  int counter = 0;

  /* Infinite loop */
  tick_count = osKernelGetTickCount();
  tick_update = osKernelGetTickFreq() / BATTERY_SAMPLE_RATE;

  /* ADC init */

  HAL_ADC_Stop_DMA(&hadc1);
  HAL_ADC_Start_DMA(&hadc1, adc_value, 4);

  osDelay(500);

  while (1) {
    tick_count += tick_update;
    double current2 = ((double)adc_value[0] * (2.45 / 65536.0) - (3.41 * 0.1)) /
                      0.66;                                       // CURR2
    double current1 = ((double)adc_value[1] * (2.45 / 65536.0) - (3.41 * 0.1)) /
                          0.66;  // CURR1

    float supply_voltage = adc_value[2] * (2.45 / 65536.0) * 2;           // 3V3
    float battery_voltage = adc_value[3] * (2.45 / 65536.0) * 5.2;  // BAT


    if ((adc_value[0] | adc_value[1]) == 0) {
      HAL_ADC_Stop_DMA(&hadc1);
      HAL_ADC_Start_DMA(&hadc1, adc_value, 4);
    }
    // Filter adc values
    if (counter < 50) {
    	current_buffer[counter] = current1+current2;
    	battery_buffer[counter] = battery_voltage;
    	supply_buffer[counter] = supply_voltage;
    	counter++;
    } else {
    	counter = 0;
    }
    if(counter%10 == 1){
    	curr = 0;
    	supp = 0;
    	bat = 0;

    	for (int i = 0; i < 50; i++){
    		curr += current_buffer[i];
    		supp += supply_buffer[i];
    		bat += battery_buffer[i];
    	}

    	mah += (curr) / (BATTERY_SAMPLE_RATE * 3.6);
		battery_data.consumption = (uint16_t)mah;
		battery_data.current = (uint16_t)((curr*20));

		battery_data.supply = (uint16_t)(supp * 20);
		battery_data.battery = (uint16_t)(bat * 20);
		battery_data.power = (curr/50 * bat/50)*1000;

		usb_print(
		  "[BATTERY] Supply: %d Battery: %d Current: %dmA, Consumption: %dmAh "
		  "Power: %dmW\n",
		  battery_data.supply, battery_data.battery, battery_data.current,
		  battery_data.consumption, battery_data.power);

		/* Log Battery Power */
		log_sensor(osKernelGetTickCount(), 4, BATTERY, &battery_data);

		/* Write Data into global Variable */
		if (acquire_mutex(&battery_mutex) == osOK) {
		global_battery_data.battery = battery_data.battery;
		global_battery_data.consumption = battery_data.consumption;
		global_battery_data.current = battery_data.current;
		release_mutex(&battery_mutex);
		}
    }
    /* Sleep */
    osDelayUntil(tick_count);
  }
}
