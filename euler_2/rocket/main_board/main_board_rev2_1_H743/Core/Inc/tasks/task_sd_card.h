/*
 * task_sd_card.h
 *
 *  Created on: Feb 26, 2020
 *      Author: stoja
 */

#ifndef INC_TASKS_TASK_SD_CARD_H_
#define INC_TASKS_TASK_SD_CARD_H_

#include "util/mutex.h"
#include "util/util.h"

#define SD_CARD_SYNC_COUNT 4


extern custom_mutex_t fsm_mutex;
extern flight_phase_detection_t global_flight_phase_detection;

extern uint8_t sd_card_logging_status;

void vTaskSdCard(void *argument);

extern SD_HandleTypeDef hsd1;

#endif /* INC_TASKS_TASK_SD_CARD_H_ */
