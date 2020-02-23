/*
 * typedef.c
 *
 *  Created on: Dec 26, 2019
 *      Author: stoja
 */

#include "typedef.h"

void vinit_queue(osMessageQueueId_t queue)
{
	queue = osMessageQueueNew(QUEUE_SIZE, sizeof(imu_data), NULL);
}
