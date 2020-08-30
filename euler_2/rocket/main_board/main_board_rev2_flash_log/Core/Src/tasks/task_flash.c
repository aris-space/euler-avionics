/*
 * task_flash.c
 *
 *  Created on: Aug 20, 2020
 *      Author: Jonas
 */
#include "tasks/task_flash.h"
#include "Flash/w25qxx.h"
#include <stdlib.h>
#include <stdio.h>

void vTaskFlash(void *argument) {
	/* For periodic update */
	uint32_t tick_count, tick_update;

	osDelay(500);

	uint8_t *logf_buffer = (uint8_t*) calloc(4096, sizeof(uint8_t));
	uint16_t logf_buffer_idx = 0;

	uint8_t *logf_buffer_read = (uint8_t*) calloc(4096, sizeof(uint8_t));

	tick_update = osKernelGetTickFreq() / FLASH_SAMPLING_FREQ;
	tick_count = osKernelGetTickCount();

	//flash_data_t data_read = { .data = { -1 }, .s = { 'd', 'e', 'f' } };

	UsbPrint("[FLASH] Init Begin...\n");
	W25qxx_Init();
	UsbPrint("[FLASH] Page Size: %d Bytes\n", w25qxx.PageSize);
	osDelay(1);
	UsbPrint("[FLASH] Page Count: %d\n", w25qxx.PageCount);
	osDelay(1);
	UsbPrint("[FLASH] Sector Size: %d Bytes\n", w25qxx.SectorSize);
	osDelay(1);
	UsbPrint("[FLASH] Sector Count: %d\n", w25qxx.SectorCount);
	osDelay(1);
	UsbPrint("[FLASH] Block Size: %d Bytes\n", w25qxx.BlockSize);
	osDelay(1);
	UsbPrint("[FLASH] Block Count: %d\n", w25qxx.BlockCount);
	osDelay(1);
	UsbPrint("[FLASH] Capacity: %d KiloBytes\n", w25qxx.CapacityInKiloByte);
	osDelay(1);
	UsbPrint("[FLASH] Init Done\n");
	osDelay(1);
	UsbPrint("[FLASH] Erasing data on chip\n");
	W25qxx_EraseChip();
	UsbPrint("[FLASH] Data on chip erased!\n");

//	[FLASH] Page Size: 256 Bytes
//	[FLASH] Page Count: 65536
//	[FLASH] Sector Size: 4096 Bytes
//	[FLASH] Sector Count: 4096
//	[FLASH] Block Size: 65536 Bytes
//	[FLASH] Block Count: 256
//	[FLASH] Capacity: 16384 KiloBytes

//uint32_t page_id = 0;
	uint16_t sector_id = 0;

	osDelay(10000);
	while (1) {
		/* Tick Update */
		tick_count += tick_update;
		flash_log_elem_t curr_log_elem = { 0 };
		//flash_data_t flash_data = { 0 };

		while (logf_buffer_idx <= (4096 - sizeof(flash_log_elem_t))) {
			if (osMessageQueueGet(logf_queue, &curr_log_elem, NULL,
			osWaitForever) == osOK) {
				UsbPrint("Queue size:%d", osMessageQueueGetCount(logf_queue));
				uint16_t log_elem_size = 0;
				uint8_t log_type_uint8_t = (uint8_t) curr_log_elem.log_type;
				logf_buffer[logf_buffer_idx++] = log_type_uint8_t;
				switch (curr_log_elem.log_type) {
				case SENSOR: {
					log_elem_size = sizeof(curr_log_elem.u.sensor_log);
				}
					break;
				case MSG: {
					/* insert string length before string itself -- limit 255 chars + null terminator */
					uint8_t string_length = (uint8_t) strlen(
							curr_log_elem.u.str) + 1;
					logf_buffer[logf_buffer_idx++] = string_length;
					log_elem_size = string_length;
				}
					break;
				default:
					UsbPrint("[FLASH] Log type not recognized!\n");
				}
				memcpy(&(logf_buffer[logf_buffer_idx]), &curr_log_elem.u,
						log_elem_size);
				logf_buffer_idx += log_elem_size;
			}
		}
		/* reset log buffer index */
		logf_buffer_idx = 0;
		UsbPrint("[FLASH] logf_buffer_filled.\n");
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		W25qxx_WriteSector(logf_buffer, sector_id, 0, 4096);
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		UsbPrint("[FLASH] Sector %hu filled.\n", sector_id);

		if (sector_id == 4095) {
			sector_id = 0;
			UsbPrint("[FLASH] All sectors written, erasing the chip...\n");
			W25qxx_EraseChip();
			UsbPrint("[FLASH] Data on chip erased!\n");
		} else {
			W25qxx_ReadSector(logf_buffer_read, sector_id, 0, 4096);
			uint16_t logf_buffer_read_idx = 0;
			uint16_t log_elem_size = 0;
			char print_buf[200] = { 0 };
			while (logf_buffer_read_idx < 4096) {
				uint8_t log_type_uint8_t =
						logf_buffer_read[logf_buffer_read_idx++];
				log_entry_type_e log_type = (log_entry_type_e) log_type_uint8_t;
				switch (log_type) {
				case SENSOR: {
					log_elem_size = sizeof(sensor_log_elem_t);
					sensor_log_elem_t sensor_log;
					memcpy(&sensor_log, &logf_buffer_read[logf_buffer_read_idx],
							log_elem_size);
					snprintf(print_buf + strlen(print_buf), 200,
							"[FLASH]: Sector#: %hu, Page#: %hu; Sensor data: %lu;%d;%hi,%d,",
							sector_id, logf_buffer_read_idx / 256,
							sensor_log.ts, SENSOR, sensor_log.sensor_board_id,
							sensor_log.sens_type);
					switch (sensor_log.sens_type) {
					case BARO: {
						baro_data_t *baro_data_ptr =
								(baro_data_t*) &sensor_log.sensor_data.baro;
						snprintf(print_buf + strlen(print_buf), 200,
								"P: %ld,T: %ld,Ts: %lu\n",
								baro_data_ptr->pressure,
								baro_data_ptr->temperature, baro_data_ptr->ts);
					}
						break;
					case IMU: {
						imu_data_t *imu_data_ptr =
								(imu_data_t*) &sensor_log.sensor_data.imu;
						snprintf(print_buf + strlen(print_buf), 200,
								"Ax: %hd, Ay: %hd, Az: %hd,Gx: %hd,Gy: %hd,Gz: %hd,Ts: %lu\n",
								imu_data_ptr->acc_x, imu_data_ptr->acc_y,
								imu_data_ptr->acc_z, imu_data_ptr->gyro_x,
								imu_data_ptr->gyro_y, imu_data_ptr->gyro_z,
								imu_data_ptr->ts);
					}
						break;
					case BATTERY: {
						battery_data_t *battery_data_ptr =
								(battery_data_t*) &sensor_log.sensor_data.bat;
						snprintf(print_buf + strlen(print_buf), 200,
								"%hd,%hd,%hd,%hd\n", battery_data_ptr->battery,
								battery_data_ptr->consumption,
								battery_data_ptr->current,
								battery_data_ptr->supply);
						break;
					}
					default: {
						snprintf(print_buf + strlen(print_buf), 200,
								"[FLASH] Sensor type not recognized!\n");
						break;
					}
					}
				}
					break;
				case MSG: {
					/* first elem of log msg is its size */
					log_elem_size = logf_buffer_read[logf_buffer_read_idx++];
					char str_log[LOG_BUFFER_LEN];
					memcpy(&str_log, &logf_buffer_read[logf_buffer_read_idx],
							log_elem_size);
					snprintf(print_buf + strlen(print_buf), 200,
							"[FLASH]: Sector#: %hu, Page#: %hu; Strlen: %d, Msg: %s\n",
							sector_id, logf_buffer_read_idx / 256,
							log_elem_size, str_log);
				}
					break;
				default: {
					logf_buffer_read_idx = 4096;
					snprintf(print_buf + strlen(print_buf), 200,
							"[FLASH] Log type not recognized while reading!\n");
				}
				}
				logf_buffer_read_idx += log_elem_size;
				UsbPrint(print_buf);
				print_buf[0] = 0;
			}
			sector_id++;
		}

		/* Sleep */
		osDelayUntil(tick_count);
	}

}
