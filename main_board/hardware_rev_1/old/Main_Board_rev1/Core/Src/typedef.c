/*
 * typedef.c
 *
 *  Created on: Nov 9, 2019
 *      Author: Jonas
 */

#include "typedef.h"

void vinit_queue(osMessageQueueId_t queue)
{
	queue = osMessageQueueNew(QUEUE_SIZE, sizeof(imu_data), NULL);
}
