/*
 * task_sd_card.c
 *
 *  Created on: Feb 26, 2020
 *      Author: stoja
 */

#include "fatfs.h"

#include "tasks/task_sd_card.h"
#include "Util/util.h"
#include "Flash/w25qxx.h"

#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern Disk_drvTypeDef disk;

FATFS EULER_FatFS;
FIL EULER_LOG_FILE;

uint8_t transformFRESULT(FRESULT res) {
	switch (res) {
	case FR_OK: /* (0) Succeeded */
		return 0;
		break;
	case FR_DISK_ERR: /* (1) A hard error occurred in the low level disk I/O layer */
		return 1;
		break;
	case FR_INT_ERR: /* (2) Assertion failed */
		return 2;
		break;
	case FR_NOT_READY: /* (3) The physical drive cannot work */
		return 3;
		break;
	case FR_NO_FILE: /* (4) Could not find the file */
		return 4;
		break;
	case FR_NO_PATH: /* (5) Could not find the path */
		return 5;
		break;
	case FR_INVALID_NAME: /* (6) The path name format is invalid */
		return 6;
		break;
	case FR_DENIED: /* (7) Access denied due to prohibited access or directory full */
		return 7;
		break;
	case FR_EXIST: /* (8) Access denied due to prohibited access */
		return 8;
		break;
	case FR_INVALID_OBJECT: /* (9) The file/directory object is invalid */
		return 9;
		break;
	case FR_WRITE_PROTECTED: /* (10) The physical drive is write protected */
		return 10;
		break;
	case FR_INVALID_DRIVE: /* (11) The logical drive number is invalid */
		return 11;
		break;
	case FR_NOT_ENABLED: /* (12) The volume has no work area */
		return 12;
		break;
	case FR_NO_FILESYSTEM: /* (13) There is no valid FAT volume */
		return 13;
		break;
	case FR_MKFS_ABORTED: /* (14) The f_mkfs() aborted due to any problem */
		return 14;
		break;
	case FR_TIMEOUT: /* (15) Could not get a grant to access the volume within defined period */
		return 15;
		break;
	case FR_LOCKED: /* (16) The operation is rejected according to the file sharing policy */
		return 16;
		break;
	case FR_NOT_ENOUGH_CORE: /* (17) LFN working buffer could not be allocated */
		return 17;
		break;
	case FR_TOO_MANY_OPEN_FILES: /* (18) Number of open files > _FS_LOCK */
		return 18;
		break;
	case FR_INVALID_PARAMETER:
		return 19;
		break;
	}
	return 20;
}

void mountSDCard() {
	FRESULT res;
	do {
		res = f_mount(&EULER_FatFS, "", 1);
		if (res != FR_OK) {
			UsbPrint("[STORAGE TASK] Failed mounting SD card: %d\n", res);
			// force sd card to be reinitialized
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
			osDelay(10);
		}
	} while (res != FR_OK);
}

void remountSDCard() {
	FRESULT res;
	//f_close(&EULER_LOG_FILE);
	do {
		MX_FATFS_DeInit();
		osDelay(50);
		MX_FATFS_Init();
		osDelay(50);
//		f_mount(0, "", 0);
//		memset(&EULER_FatFS, 0, sizeof(EULER_FatFS));
		res = f_mount(&EULER_FatFS, "", 1);
		if (res != FR_OK) {
			UsbPrint("[STORAGE TASK] Failed remounting SD card: %d\n", res);
			// force sd card to be reinitialized
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
			osDelay(10);
		}
	} while (res != FR_OK);
}

FRESULT findNextFileName(char *file_name) {
	FRESULT res;
	UsbPrint("[STORAGE TASK] Creating file name\n");
	uint16_t file_number = 1;

	DIR dj;
	FILINFO fno;
	res = f_findfirst(&dj, &fno, "", "LOG_???.CSV");
	while (res == FR_OK && fno.fname[0]) {
		uint16_t current_file_number = (fno.fname[4] - '0') * 100
				+ (fno.fname[5] - '0') * 10 + (fno.fname[6] - '0');
		if (current_file_number + 1 > file_number) {
			file_number = current_file_number + 1;
		}
		res = f_findnext(&dj, &fno);
	}
	file_number = file_number <= 999 ? file_number : 999;

	if (res == FR_OK) {
		strcpy(file_name, "LOG_000.CSV");
		file_name[6] = '0' + file_number % 10;
		file_name[5] = '0' + (file_number / 10) % 10;
		file_name[4] = '0' + (file_number / 100) % 10;

		UsbPrint("[STORAGE TASK] Using file name: %s\n", file_name);

		res = f_closedir(&dj);
		if (res != FR_OK) {
			UsbPrint("[STORAGE TASK] Failed closing directory: %d\n", res);
		}
	} else {
		UsbPrint("[STORAGE TASK] Failed finding first or next file: %d\n", res);
	}

	return res;
}

int32_t formatLogStr(char *str, log_elem_t *log_entry) {
	int32_t str_len = 0;
	switch (log_entry->log_type) {
	case SENSOR: {
		sensor_log_elem_t *sensor_log_entry = &(log_entry->u.sensor_log);
		sensor_type_e sensor_type = sensor_log_entry->sens_type;
		str_len = snprintf(str, SD_STRFMT_LEN, "%lu;%d;%hi,%d,", log_entry->ts,
				SENSOR, sensor_log_entry->sensor_board_id, sensor_type);
		switch (sensor_type) {
		case BARO: {
			baro_data_t *baro_data_ptr = &(sensor_log_entry->sensor_data.baro);
			str_len += snprintf(str + str_len,
					SD_STRFMT_LEN, "%ld,%ld,%lu\n", baro_data_ptr->pressure,
					baro_data_ptr->temperature, baro_data_ptr->ts);
		}
			break;
		case IMU: {
			imu_data_t *imu_data_ptr = &(sensor_log_entry->sensor_data.imu);
			str_len += snprintf(str + str_len,
					SD_STRFMT_LEN, "%hd,%hd,%hd,%hd,%hd,%hd,%lu\n",
					imu_data_ptr->acc_x, imu_data_ptr->acc_y,
					imu_data_ptr->acc_z, imu_data_ptr->gyro_x,
					imu_data_ptr->gyro_y, imu_data_ptr->gyro_z,
					imu_data_ptr->ts);
		}
			break;
		case GPS: {
			gps_data_t *gps_data = &(sensor_log_entry->sensor_data.gps);
			str_len += snprintf(str + str_len,
					SD_STRFMT_LEN, "%ld,%ld,%ld,%d,%ld,%d,%ld,%d,%hd,%hd\n",
					gps_data->hour, gps_data->minute, gps_data->second,
					gps_data->lat_deg, gps_data->lat_decimal, gps_data->lon_deg,
					gps_data->lon_decimal, gps_data->satellite,
					gps_data->altitude, gps_data->HDOP);
		}
			break;
		case BATTERY: {
			battery_data_t *battery_data = &(sensor_log_entry->sensor_data.bat);
			str_len += snprintf(str + str_len,
					SD_STRFMT_LEN, "%hd,%hd,%hd,%hd\n", battery_data->battery,
					battery_data->consumption, battery_data->current,
					battery_data->supply);
		}
			break;
		default:
			break;
		}
	}
		break;
	case STATE: {
		str_len = snprintf(str, SD_STRFMT_LEN, "%lu;%d;%d\n", log_entry->ts,
				STATE, log_entry->u.state.flight_phase);
	}
		break;
	case ESTIMATOR_VAR: {
		str_len = snprintf(str, SD_STRFMT_LEN, "%lu;%d;%ld,%ld,%ld\n",
				log_entry->ts, ESTIMATOR_VAR,
				log_entry->u.est_var.position_world[2],
				log_entry->u.est_var.velocity_rocket[0],
				log_entry->u.est_var.acceleration_rocket[0]);
	}
		break;
	case CONTROLLER_OUTPUT: {
		str_len = snprintf(str, SD_STRFMT_LEN, "%lu;%d;%ld,%ld,%ld\n",
				log_entry->ts, CONTROLLER_OUTPUT,
				log_entry->u.cont_out.controller_output,
				log_entry->u.cont_out.reference_error,
				log_entry->u.cont_out.integrated_error);
	}
		break;
	case MOTOR_POSITION: {
		str_len = snprintf(str, SD_STRFMT_LEN, "%lu;%d;%ld,%ld\n",
				log_entry->ts, MOTOR_POSITION,
				log_entry->u.motor.desired_position,
				log_entry->u.motor.actual_position);
	}
		break;
	case MSG: {
		str_len = snprintf(str, SD_STRFMT_LEN, "%lu;%d;%s\n", log_entry->ts,
				MSG, log_entry->u.msg);
	}
		break;
	}
	return str_len;
}

FRESULT openFile(char *file_name) {
	FRESULT res;
	UsbPrint("[STORAGE TASK] Opening log file\n");
	res = f_open(&EULER_LOG_FILE, file_name,
	FA_OPEN_ALWAYS | FA_WRITE);
	if (res != FR_OK) {
		UsbPrint("[STORAGE TASK] Failed opening log file \"%s\": %d\n",
				file_name, res);
		return res;
	}

	UsbPrint("[STORAGE TASK] Going to end of file\n");
	res = f_lseek(&EULER_LOG_FILE, f_size(&EULER_LOG_FILE));
	if (res != FR_OK) {
		UsbPrint("[STORAGE TASK] Failed going to end of file: %d\n", res);
		return res;
	}
	return res;
}

void vTaskSdCard(void *argument) {
	// Try everything forever;
	UsbPrint("[STORAGE TASK] Starting SD Card Task...\n");
	FRESULT res;
	char EULER_LOG_FILE_NAME[13] = "";
	char *sd_str_buffer = (char*) calloc(SD_BUFFER_LEN, sizeof(char));
	uint16_t sd_str_idx = 0;
	osDelay(3000);
	UsbPrint("[STORAGE TASK] Mounting SD Card...\n");
	mountSDCard();
	uint8_t already_entered = 0;
	for (;;) {
		osDelay(100);
		flight_phase_detection_t local_flight_phase = { .flight_phase = IDLE };

		if (already_entered) {
			UsbPrint("[STORAGE TASK] Remounting SD Card...\n");
			remountSDCard();
		} else {
			already_entered = 1;
		}
		/* TODO [nemanja]: change this goto */
		logToNewFile: if (findNextFileName(EULER_LOG_FILE_NAME) != FR_OK) {
			UsbPrint("[STORAGE TASK] Error while finding log file!\n");
			continue;
		}

		if (openFile(EULER_LOG_FILE_NAME) != FR_OK) {
			continue;
		}

		char log_header[32] = "Timestamp;Log Entry Type;Data\n";
		int32_t puts_res = f_puts(log_header, &EULER_LOG_FILE);
		if (puts_res < 0) {
			UsbPrint("[STORAGE TASK] Failed writing to file: %d\n", puts_res);
			continue; /* continuing for loop */
		}

		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);

		volatile int32_t sync_counter = 0;
		sd_str_idx = 0;
		char tmp_str[SD_STRFMT_LEN];
		log_elem_t curr_log_elem = { 0 };
		int32_t tmp_str_len = 0;
		for (;;) {
			/* get log element */
			if (osMessageQueueGet(sd_queue, &curr_log_elem, NULL,
			osWaitForever) == osOK) {
				/* prepare tmp log string */
				tmp_str_len = formatLogStr(tmp_str, &curr_log_elem);
				if (tmp_str_len > 0) {
					if (sd_str_idx + tmp_str_len < SD_BUFFER_LEN) {
						/* write output */
						strncpy(sd_str_buffer + sd_str_idx, tmp_str,
								tmp_str_len);
//						memcpy(sd_str_buffer + sd_str_idx, tmp_str,
//								tmp_str_len);
						//UsbPrint("%s", tmp_str);
						sd_str_idx += tmp_str_len;
						tmp_str[0] = 0;
						//memset(tmp_str, 0, 150);
					} else {
						//UsbPrint("[STORAGE TASK] sd_str_idx: %d\n", sd_str_idx);
//						osDelay(2);
//						int32_t i = 0;
//						while (i < sd_str_idx - 300) {
//							UsbPrint("%s", sd_str_buffer + i);
//							osDelay(2);
//							i += 300;
//						}
						int32_t j = 0;
						while (j < sd_str_idx) {
							if (sd_str_buffer[j] == 0) {
								UsbPrint(
										"[STORAGE TASK] TERMINATOR DETECTED BEFORE END: %d\n",
										j);
								osDelay(2);
							}
							j++;
						}
						sd_str_buffer[sd_str_idx++] = '\0';
						int32_t puts_res = f_puts(sd_str_buffer,
								&EULER_LOG_FILE);
						if (puts_res < 0) {
							UsbPrint(
									"[STORAGE TASK] Failed writing to file: %d\n",
									puts_res);
							break; /* breaking out of inner for loop */
						}
						sync_counter++;
						if (sync_counter >= SD_CARD_SYNC_COUNT) {
							sync_counter = 0;
							//UsbPrint("[STORAGE TASK] Syncing file %s\n",
							//		EULER_LOG_FILE_NAME);
							HAL_GPIO_TogglePin(LED4_GPIO_Port,
							LED4_Pin);
							res = f_sync(&EULER_LOG_FILE);
							if (res != FR_OK) {
								UsbPrint(
										"[STORAGE TASK] Failed syncing file: %d\n",
										res);
								break; /* breaking out of inner for loop */
							}
							// if the rocket landed, create a new file and write to that one
							if (ReadMutex(&fsm_mutex,
									&global_flight_phase_detection,
									&local_flight_phase,
									sizeof(global_flight_phase_detection))
									== osOK
									&& local_flight_phase.flight_phase
											== RECOVERY) {
								f_close(&EULER_LOG_FILE);
								// "clean" current file name
								EULER_LOG_FILE_NAME[0] = 0;
								goto logToNewFile;
							}
						}
						/* fill buffer with leftover tmp_str */

						//memset(sd_str_buffer, 0, SD_BUFFER_LEN);
						strncpy(sd_str_buffer, tmp_str, tmp_str_len);
						//memcpy(sd_str_buffer, tmp_str, tmp_str_len);
						sd_str_idx = tmp_str_len;
						tmp_str[0] = 0;
						//memset(tmp_str, 0, 150);
					}
				} else {
					UsbPrint("[STORAGE TASK] tmp_str_len < 0!!!!!! %d\n",
							tmp_str_len);
					tmp_str_len = 0;
					tmp_str[0] = 0;
					//memset(tmp_str, 0, 150);
				}
			}
		}
	}
	free(sd_str_buffer);
}
