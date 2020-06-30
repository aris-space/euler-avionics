/*
 * task_battery.h
 *
 *  Created on: Jun 26, 2020
 *      Author: Luca
 */

#ifndef INC_TASKS_TASK_BATTERY_H_
#define INC_TASKS_TASK_BATTERY_H_

#include "stm32f7xx_hal.h"

#define BATTERY_SAMPLE_RATE 500

extern ADC_HandleTypeDef hadc1;


void vTaskBattery(void *argument);


#endif /* INC_TASKS_TASK_BATTERY_H_ */
