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

	uint8_t *flash_buffer = (uint8_t*) calloc(FLASH_BUFFER_LEN,
			sizeof(uint8_t));
	uint16_t flash_buffer_idx = 0;

#ifdef DEBUG
	uint8_t *flash_read_buffer = (uint8_t*) calloc(FLASH_BUFFER_LEN,
			sizeof(uint8_t));
	uint16_t flash_read_buffer_idx = 0;
#endif

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
		log_elem_t curr_log_elem = { 0 };
		//flash_data_t flash_data = { 0 };

		while (flash_buffer_idx < (FLASH_BUFFER_LEN - sizeof(log_elem_t))) {
			if (osMessageQueueGet(flash_queue, &curr_log_elem, NULL,
			osWaitForever) == osOK) {
				//UsbPrint("Queue size:%d", osMessageQueueGetCount(logf_queue));
				uint16_t log_elem_size = sizeof(curr_log_elem);
				/* write timestmap */
				memcpy(&(flash_buffer[flash_buffer_idx]), &curr_log_elem,
						sizeof(curr_log_elem));
//				memcpy(&(flash_buffer[flash_buffer_idx]), &curr_log_elem.ts,
//						sizeof(curr_log_elem.ts));
//				flash_buffer_idx += sizeof(curr_log_elem.ts);
//				/* write log type */
//				uint8_t log_type_uint8_t = (uint8_t) curr_log_elem.log_type;
//				flash_buffer[flash_buffer_idx++] = log_type_uint8_t;
//				/* write log element */
//				switch (curr_log_elem.log_type) {
//				case SENSOR: {
//					//log_elem_size = sizeof(curr_log_elem.u.sensor_log);
//					memcpy(&(flash_buffer[flash_buffer_idx]),
//							&curr_log_elem.u.sensor_log.sensor_board_id,
//							sizeof(curr_log_elem.u.sensor_log.sensor_board_id));
//					flash_buffer_idx +=
//							sizeof(curr_log_elem.u.sensor_log.sensor_board_id);
//
//					memcpy(&(flash_buffer[flash_buffer_idx]),
//							&curr_log_elem.u.sensor_log.sens_type,
//							sizeof(curr_log_elem.u.sensor_log.sens_type));
//					flash_buffer_idx +=
//							sizeof(curr_log_elem.u.sensor_log.sens_type);
//
//					switch (curr_log_elem.u.sensor_log.sens_type) {
//					case BARO: {
//						log_elem_size =
//								sizeof(curr_log_elem.u.sensor_log.sensor_data.baro);
//					}
//						break;
//					case IMU: {
//						log_elem_size =
//								sizeof(curr_log_elem.u.sensor_log.sensor_data.imu);
//					}
//						break;
//					case GPS: {
//						log_elem_size =
//								sizeof(curr_log_elem.u.sensor_log.sensor_data.gps);
//					}
//						break;
//					case BATTERY: {
//						log_elem_size =
//								sizeof(curr_log_elem.u.sensor_log.sensor_data.bat);
//					}
//						break;
//					default:
//						UsbPrint("[FLASH-WRITE] Bad sensor type!\n");
//						break;
//					}
//					memcpy(&(flash_buffer[flash_buffer_idx]),
//							&curr_log_elem.u.sensor_log.sensor_data,
//							log_elem_size);
//
//				}
//					break;
//				case STATE: {
//					log_elem_size = sizeof(curr_log_elem.u.state);
//				}
//					break;
//				case ESTIMATOR_VAR: {
//					log_elem_size = sizeof(curr_log_elem.u.est_var);
//				}
//					break;
//				case CONTROLLER_OUTPUT: {
//					log_elem_size = sizeof(curr_log_elem.u.cont_out);
//				}
//					break;
//				case MOTOR_POSITION: {
//					log_elem_size = sizeof(curr_log_elem.u.motor);
//				}
//					break;
//				case MSG: {
//					/* insert string length before string itself -- limit 255 chars + null terminator */
//					uint8_t string_length = (uint8_t) strlen(
//							curr_log_elem.u.msg) + 1;
//					flash_buffer[flash_buffer_idx++] = string_length;
//					log_elem_size = string_length;
//				}
//					break;
//				default:
//					UsbPrint("[FLASH-WRITE] Log type not recognized!\n");
//					break;
//				}
//				if (curr_log_elem.log_type != SENSOR) {
//					memcpy(&(flash_buffer[flash_buffer_idx]), &curr_log_elem.u,
//							log_elem_size);
//				}
				flash_buffer_idx += log_elem_size;
			}
		}
		/* reset log buffer index */
		flash_buffer_idx = 0;
		UsbPrint("[FLASH] flash_buffer filled.\n");
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		W25qxx_WriteSector(flash_buffer, sector_id, 0, FLASH_BUFFER_LEN);
#ifdef DEBUG
		osDelay(10);
		W25qxx_ReadSector(flash_read_buffer, sector_id, 0, FLASH_BUFFER_LEN);
		flash_read_buffer_idx = 0;
		char print_buf[200] = { 0 };
		//timestamp_t ts = { 0 };
		while (flash_read_buffer_idx < (FLASH_BUFFER_LEN - sizeof(log_elem_t))) {
			memcpy(&curr_log_elem, &flash_read_buffer[flash_read_buffer_idx],
					sizeof(curr_log_elem));
			flash_read_buffer_idx += sizeof(curr_log_elem);
			switch (curr_log_elem.log_type) {
			case SENSOR: {
				snprintf(print_buf, 200,
						"[FLASH]: Sector#: %hu, Page#: %hu; Data: %lu;%d;%hi,%d | Sensor data: ",
						sector_id, flash_read_buffer_idx / 256,
						curr_log_elem.ts, SENSOR,
						curr_log_elem.u.sensor_log.sensor_board_id,
						curr_log_elem.u.sensor_log.sens_type);

				switch (curr_log_elem.u.sensor_log.sens_type) {
				case BARO: {
					snprintf(print_buf + strlen(print_buf), 200,
							"P: %ld,T: %ld,Ts: %lu\n",
							curr_log_elem.u.sensor_log.sensor_data.baro.pressure,
							curr_log_elem.u.sensor_log.sensor_data.baro.temperature,
							curr_log_elem.u.sensor_log.sensor_data.baro.ts);
				}
					break;

				case IMU: {
					snprintf(print_buf + strlen(print_buf), 200,
							"Ax: %hd, Ay: %hd, Az: %hd,Gx: %hd,Gy: %hd,Gz: %hd,Ts: %lu\n",
							curr_log_elem.u.sensor_log.sensor_data.imu.acc_x,
							curr_log_elem.u.sensor_log.sensor_data.imu.acc_y,
							curr_log_elem.u.sensor_log.sensor_data.imu.acc_z,
							curr_log_elem.u.sensor_log.sensor_data.imu.gyro_x,
							curr_log_elem.u.sensor_log.sensor_data.imu.gyro_y,
							curr_log_elem.u.sensor_log.sensor_data.imu.gyro_z,
							curr_log_elem.u.sensor_log.sensor_data.imu.ts);
				}
					break;

				case GPS: {
				}
					break;

				case BATTERY: {

					snprintf(print_buf + strlen(print_buf), 200,
							"BAT: %hd,CONS: %hd,CURR: %hd,SUPPLY: %hd\n",
							curr_log_elem.u.sensor_log.sensor_data.bat.battery,
							curr_log_elem.u.sensor_log.sensor_data.bat.consumption,
							curr_log_elem.u.sensor_log.sensor_data.bat.current,
							curr_log_elem.u.sensor_log.sensor_data.bat.supply);
				}
					break;
				default:
					UsbPrint("[FLASH-READ] Bad sensor type!\n");
					break;
				}
				//UsbPrint("Sensor...\n");
			}
				break;
			case MSG: {
				//UsbPrint("Msg...\n");
			}
				break;
			default:
				UsbPrint("[FLASH-READ] Bad log type!\n");
				break;
			}
			UsbPrint(print_buf);
			//osDelay(20);
			print_buf[0] = 0;
//			memcpy(&ts, &flash_read_buffer[flash_read_buffer_idx],
//					sizeof(curr_log_elem.ts));
//			flash_read_buffer_idx += sizeof(curr_log_elem.ts);
//			uint8_t log_type_uint8_t =
//					flash_read_buffer[flash_read_buffer_idx++];
//			log_entry_type_e log_type = (log_entry_type_e) log_type_uint8_t;
//			switch (log_type) {
//			case SENSOR: {
//				board_id_t sensor_board_id;
//				sensor_type_e sens_type;
//				memcpy(&sensor_board_id,
//						&flash_read_buffer[flash_read_buffer_idx],
//						sizeof(curr_log_elem.u.sensor_log.sensor_board_id));
//				flash_read_buffer_idx +=
//						sizeof(curr_log_elem.u.sensor_log.sensor_board_id);
//
//				memcpy(&sens_type, &flash_read_buffer[flash_read_buffer_idx],
//						sizeof(curr_log_elem.u.sensor_log.sens_type));
//				flash_read_buffer_idx +=
//						sizeof(curr_log_elem.u.sensor_log.sens_type);
//
//				snprintf(print_buf + strlen(print_buf), 200,
//						"[FLASH]: Sector#: %hu, Page#: %hu; Data: %lu;%d;%hi,%d | Sensor data: ",
//						sector_id, flash_read_buffer_idx / 256, ts, SENSOR,
//						sensor_board_id, sens_type);
//
//				switch (sens_type) {
//				case BARO: {
//					baro_data_t baro;
//					memcpy(&baro, &flash_read_buffer[flash_read_buffer_idx],
//							sizeof(baro));
//					snprintf(print_buf + strlen(print_buf), 200,
//							"P: %ld,T: %ld,Ts: %lu\n", baro.pressure,
//							baro.temperature, baro.ts);
//					flash_read_buffer_idx += sizeof(baro);
//				}
//					break;
//
//				case IMU: {
//					imu_data_t imu;
//					memcpy(&imu, &flash_read_buffer[flash_read_buffer_idx],
//							sizeof(imu));
//					snprintf(print_buf + strlen(print_buf), 200,
//							"Ax: %hd, Ay: %hd, Az: %hd,Gx: %hd,Gy: %hd,Gz: %hd,Ts: %lu\n",
//							imu.acc_x, imu.acc_y, imu.acc_z, imu.gyro_x,
//							imu.gyro_y, imu.gyro_z, imu.ts);
//					flash_read_buffer_idx += sizeof(imu);
//				}
//					break;
//
//				case GPS: {
//					gps_data_t gps;
//					memcpy(&gps, &flash_read_buffer[flash_read_buffer_idx],
//							sizeof(gps));
////					snprintf(print_buf + strlen(print_buf), 200,
////							"P: %ld,T: %ld,Ts: %lu\n", baro.pressure,
////							baro.temperature, baro.ts);
//					flash_read_buffer_idx += sizeof(gps);
//				}
//					break;
//
//				case BATTERY: {
//					battery_data_t bat;
//					memcpy(&bat, &flash_read_buffer[flash_read_buffer_idx],
//							sizeof(bat));
//					snprintf(print_buf + strlen(print_buf), 200,
//							"BAT: %hd,CONS: %hd,CURR: %hd,SUPPLY: %hd\n",
//							bat.battery, bat.consumption, bat.current,
//							bat.supply);
//					flash_read_buffer_idx += sizeof(bat);
//				}
//					break;
//				default:
//					UsbPrint("[FLASH-READ] Bad sensor type!\n");
//					break;
//				}
//				//UsbPrint("Sensor...\n");
//			}
//				break;
//			case MSG: {
//				//UsbPrint("Msg...\n");
//			}
//				break;
//			default:
//				UsbPrint("[FLASH-READ] Bad log type!\n");
//				break;
//			}
//			UsbPrint(print_buf);
//			osDelay(20);
//			print_buf[0] = 0;
		}
#endif

		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		UsbPrint("[FLASH] Sector %hu filled.\n", sector_id);

		if (sector_id == 4095) {
			sector_id = 0;
			UsbPrint("[FLASH] All sectors written!\n");
			break; /* end the task */
		} else {
			sector_id++;
		}

		/* Sleep */
		osDelayUntil(tick_count);
	}
	free(flash_buffer);
}

// TODO [nemanja] - free this thing!!! if we jump with goto we're doomed
//uint8_t *logf_buffer_read = (uint8_t*) calloc(FLASH_BUFFER_LEN,
//		sizeof(uint8_t));
//for (;;
//		) {
//			if (osMessageQueueGet(logf_sector_queue, &sector_id, NULL,
//					osWaitForever) == osOK) {
//				UsbPrint("[STORAGE TASK] Logging sector: %hu\n", sector_id);
//				W25qxx_ReadSector(logf_buffer_read, sector_id, 0, FLASH_BUFFER_LEN);
//				uint16_t logf_buffer_read_idx = 0;
//				uint16_t log_elem_size = 0;
//				char print_buf[200] = {0};
//				while (logf_buffer_read_idx < FLASH_BUFFER_LEN) {
//					uint8_t log_type_uint8_t =
//					logf_buffer_read[logf_buffer_read_idx++];
//					log_entry_type_e log_type =
//					(log_entry_type_e) log_type_uint8_t;
//					switch (log_type) {
//						case SENSOR: {
//							log_elem_size = sizeof(sensor_log_elem_t);
//							sensor_log_elem_t sensor_log;
//							memcpy(&sensor_log,
//							&logf_buffer_read[logf_buffer_read_idx],
//							log_elem_size);
//							snprintf(print_buf + strlen(print_buf), 200,
//							"[FLASH]: Sector#: %hu, Page#: %hu; Sensor data: %lu;%d;%hi,%d,",
//							sector_id, logf_buffer_read_idx / 256,
//							sensor_log.ts, SENSOR,
//							sensor_log.sensor_board_id,
//							sensor_log.sens_type);
//							switch (sensor_log.sens_type) {
//								case BARO: {
//									baro_data_t *baro_data_ptr =
//									(baro_data_t*) &sensor_log.sensor_data.baro;
//									snprintf(print_buf + strlen(print_buf), 200,
//									"P: %ld,T: %ld,Ts: %lu\n",
//									baro_data_ptr->pressure,
//									baro_data_ptr->temperature,
//									baro_data_ptr->ts);
//								}
//								break;
//								case IMU: {
//									imu_data_t *imu_data_ptr =
//									(imu_data_t*) &sensor_log.sensor_data.imu;
//									snprintf(print_buf + strlen(print_buf), 200,
//									"Ax: %hd, Ay: %hd, Az: %hd,Gx: %hd,Gy: %hd,Gz: %hd,Ts: %lu\n",
//									imu_data_ptr->acc_x,
//									imu_data_ptr->acc_y,
//									imu_data_ptr->acc_z,
//									imu_data_ptr->gyro_x,
//									imu_data_ptr->gyro_y,
//									imu_data_ptr->gyro_z, imu_data_ptr->ts);
//								}
//								break;
//								case BATTERY: {
//									battery_data_t *battery_data_ptr =
//									(battery_data_t*) &sensor_log.sensor_data.bat;
//									snprintf(print_buf + strlen(print_buf), 200,
//									"%hd,%hd,%hd,%hd\n",
//									battery_data_ptr->battery,
//									battery_data_ptr->consumption,
//									battery_data_ptr->current,
//									battery_data_ptr->supply);
//									break;
//								}
//								default: {
//									snprintf(print_buf + strlen(print_buf), 200,
//									"[FLASH] Sensor type not recognized!\n");
//									break;
//								}
//							}
//						}
//						break;
//						case MSG: {
//							// first elem of log msg is its size
//							log_elem_size = logf_buffer_read[logf_buffer_read_idx++];
//							char str_log[LOG_BUFFER_LEN];
//							memcpy(&str_log,
//							&logf_buffer_read[logf_buffer_read_idx],
//							log_elem_size);
//							snprintf(print_buf + strlen(print_buf), 200,
//							"[FLASH]: Sector#: %hu, Page#: %hu; Strlen: %d, Msg: %s\n",
//							sector_id, logf_buffer_read_idx / 256,
//							log_elem_size, str_log);
//						}
//						break;
//						default: {
//							logf_buffer_read_idx = FLASH_BUFFER_LEN;
//							snprintf(print_buf + strlen(print_buf), 200,
//							"[FLASH] Log type not recognized while reading!\n");
//						}
//					}
//					logf_buffer_read_idx += log_elem_size;
//					UsbPrint( print_buf);
//					print_buf[0] = 0;
//				}
//			}
//		}
//
