/*
 * task_battery.h
 *
 *  Created on: Jun 26, 2020
 *      Author: Luca
 */

#ifndef INC_TASKS_TASK_BATTERY_H_
#define INC_TASKS_TASK_BATTERY_H_

#include "util/mutex.h"
#include "util/telemetry_util.h"
#include "stm32h7xx_hal.h"

#define BATTERY_SAMPLE_RATE 50

#define V25 (1.43)
#define AVG_SLOPE (.0043)

extern ADC_HandleTypeDef hadc1;

extern custom_mutex_t battery_mutex;
extern telemetry_battery_data_t global_battery_data;

void vTaskBattery(void *argument);

#endif /* INC_TASKS_TASK_BATTERY_H_ */
