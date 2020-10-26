/*
 * task_sd_card.c
 *
 *  Created on: Feb 26, 2020
 *      Author: stoja
 */

#include "tasks/task_sd_card.h"
#include "util/logging_util.h"
#include "fatfs.h"
#include "main.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FATFS EULER_FatFS;
FIL EULER_LOG_FILE;
#if (configUSE_TRACE_FACILITY == 1)
	traceString sd_channel;
#endif

static void mount_sd_card();
static void remount_sd_card();
static FRESULT find_next_file_name(char *file_name);
static FRESULT open_file(char *file_name);
static int32_t format_log_str(char *str, log_elem_t *log_entry);

void vTaskSdCard(void *argument) {
  // Try everything forever;

#if (configUSE_TRACE_FACILITY == 1)
  sd_channel = xTraceRegisterString("SD Channel");
#endif

  usb_print("[STORAGE TASK] Starting SD Card Task...\n");
  FRESULT res;
  char EULER_LOG_FILE_NAME[13] = "";
  char *sd_str_buffer =
      (char *)calloc(SD_BUFFER_LEN, sizeof(char));  // 0x20000000;
  uint16_t sd_str_idx = 0;
  osDelay(300);
  usb_print("[STORAGE TASK] Mounting SD Card...\n");
  mount_sd_card();
  uint8_t already_entered = 0;
  uint8_t td_file_created = 0;
  for (;;) {
#if (configUSE_TRACE_FACILITY == 1)
    vTracePrint(sd_channel, "Starting for loop...");
#endif
    osDelay(100);
    flight_phase_detection_t local_flight_phase = {.flight_phase = IDLE};

    if (already_entered) {
      usb_print("[STORAGE TASK] Remounting SD Card...\n");
      remount_sd_card();
    } else {
      already_entered = 1;
    }

  logToNewFile:
    if (find_next_file_name(EULER_LOG_FILE_NAME) != FR_OK) {
      usb_print("[STORAGE TASK] Error while finding log file!\n");
      continue;
    }

    if (open_file(EULER_LOG_FILE_NAME) != FR_OK) {
      continue;
    }

    char log_header[32] = "Timestamp;Log Entry Type;Data\n";
    int32_t puts_res = f_puts(log_header, &EULER_LOG_FILE);
    if (puts_res < 0) {
      usb_print("[STORAGE TASK] Failed writing to file: %d\n", puts_res);
      continue; /* continuing for loop */
    }

    //HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);

    volatile int32_t sync_counter = 0;
    sd_str_idx = 0;
    char tmp_str[SD_STRFMT_LEN];
    log_elem_t curr_log_elem = {0};
    int32_t tmp_str_len = 0;
    for (;;) {
      /* get log element */
      if (osMessageQueueGet(sd_queue, &curr_log_elem, NULL, osWaitForever) ==
          osOK) {
        /* prepare tmp log string */
        tmp_str_len = format_log_str(tmp_str, &curr_log_elem);
        if (tmp_str_len > 0) {
          if ((sd_str_idx + tmp_str_len) < (SD_BUFFER_LEN - 1)) {
            /* write output */
            strncpy(sd_str_buffer + sd_str_idx, tmp_str, tmp_str_len);
            sd_str_idx += tmp_str_len;
            tmp_str[0] = 0;
          } else {
            sd_str_buffer[sd_str_idx++] = '\0';
#if (configUSE_TRACE_FACILITY == 1)
              //vTracePrint(sd_channel, "START FPUTS");
#endif
            int32_t puts_res = f_puts(sd_str_buffer, &EULER_LOG_FILE);
#if (configUSE_TRACE_FACILITY == 1)
              vTracePrint(sd_channel, "END FPUTS");
#endif
            if (puts_res < 0) {
              usb_print("[STORAGE TASK] Failed writing to file: %d\n", puts_res);
#if (configUSE_TRACE_FACILITY == 1)
              vTracePrint(sd_channel, "Writing to file failed");
#endif
              break; /* breaking out of inner for loop */
            } else {
#if (configUSE_TRACE_FACILITY == 1)
              vTracePrint(sd_channel, "Written to file");
#endif
            }
            /* TODO [nemanja]: check if this sync count is detrimental */
            if (++sync_counter >= SD_CARD_SYNC_COUNT) {
              sync_counter = 0;
              HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
              res = f_sync(&EULER_LOG_FILE);
              sd_card_logging_status = 1;
              if (res != FR_OK) {
                usb_print("[STORAGE TASK] Failed syncing file: %d\n", res);
#if (configUSE_TRACE_FACILITY == 1)
                vTracePrint(sd_channel, "File sync failed");
#endif
                break; /* breaking out of inner for loop */
              } else {
#if (configUSE_TRACE_FACILITY == 1)
                vTracePrint(sd_channel, "File synced");
#endif
              }
              // if the rocket landed, create a new file and write to that one
              if ((td_file_created == 0) && read_mutex(&fsm_mutex, &global_flight_phase_detection,
                  &local_flight_phase, sizeof(global_flight_phase_detection)) == osOK &&
                  local_flight_phase.flight_phase == TOUCHDOWN) {
            	td_file_created = 1;
                f_close(&EULER_LOG_FILE);
                // "clean" current file name
                EULER_LOG_FILE_NAME[0] = 0;
#if (configUSE_TRACE_FACILITY == 1)
                vTracePrint(sd_channel, "Logging to new file");
#endif
                usb_print("[STORAGE TASK] Touchdown detected, logging to new file!");
                goto logToNewFile;
              }
            }
            /* fill buffer with leftover tmp_str */
            strncpy(sd_str_buffer, tmp_str, tmp_str_len);
            sd_str_idx = tmp_str_len;
            tmp_str[0] = 0;
          }
        } else {
          usb_print("[STORAGE TASK] tmp_str_len < 0!!!!!! %d\n", tmp_str_len);
          tmp_str_len = 0;
          tmp_str[0] = 0;
        }
      }
    }
  }
  free(sd_str_buffer);
}

static void mount_sd_card() {
  FRESULT res;
  do {
    res = f_mount(&EULER_FatFS, "", 1);
    if (res != FR_OK) {
      usb_print("[STORAGE TASK] Failed mounting SD card: %d\n", res);
#if (configUSE_TRACE_FACILITY == 1)
      vTracePrint(sd_channel, "SD card mounting failed");
#endif
      // force sd card to be reinitialized
      //HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
      osDelay(10);
    } else {
#if (configUSE_TRACE_FACILITY == 1)
      vTracePrint(sd_channel, "SD card mounted");
#endif
    }
  } while (res != FR_OK);
}

static void remount_sd_card() {
  FRESULT res;
  // f_close(&EULER_LOG_FILE);
  sd_card_logging_status = 2;
  do {
    MX_FATFS_DeInit();
    osDelay(50);
    HAL_SD_DeInit(&hsd1);
    MX_SDMMC1_SD_Init();
    osDelay(50);
    MX_FATFS_Init();
    osDelay(50);
    memset(&EULER_FatFS, 0, sizeof(FATFS));
    res = f_mount(&EULER_FatFS, "", 1);
    if (res != FR_OK) {
      usb_print("[STORAGE TASK] Failed remounting SD card: %d\n", res);
#if (configUSE_TRACE_FACILITY == 1)
      vTracePrint(sd_channel, "SD card remounting failed");
#endif
      // force sd card to be reinitialized
      //HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
      osDelay(10);
    } else {
#if (configUSE_TRACE_FACILITY == 1)
      vTracePrint(sd_channel, "SD card remounted");
#endif
    }
  } while (res != FR_OK);
}

static FRESULT find_next_file_name(char *file_name) {
  FRESULT res;
  usb_print("[STORAGE TASK] Creating file name\n");
  uint16_t file_number = 1;

  DIR dj;
  FILINFO fno;
  res = f_findfirst(&dj, &fno, "", "LOG_???.CSV");
  while (res == FR_OK && fno.fname[0]) {
    uint16_t current_file_number = (fno.fname[4] - '0') * 100 +
                                   (fno.fname[5] - '0') * 10 +
                                   (fno.fname[6] - '0');
    if (current_file_number + 1 > file_number) {
      file_number = current_file_number + 1;
    }
    res = f_findnext(&dj, &fno);
    osDelay(10);
  }
  file_number = file_number <= 999 ? file_number : 999;

  if (res == FR_OK) {
    strcpy(file_name, "LOG_000.CSV");
    file_name[6] = '0' + file_number % 10;
    file_name[5] = '0' + (file_number / 10) % 10;
    file_name[4] = '0' + (file_number / 100) % 10;

    usb_print("[STORAGE TASK] Using file name: %s\n", file_name);

    res = f_closedir(&dj);
    if (res != FR_OK) {
      usb_print("[STORAGE TASK] Failed closing directory: %d\n", res);
    }
  } else {
    usb_print("[STORAGE TASK] Failed finding first or next file: %d\n", res);
  }

  return res;
}

static FRESULT open_file(char *file_name) {
  FRESULT res;
  usb_print("[STORAGE TASK] Opening log file\n");
  res = f_open(&EULER_LOG_FILE, file_name, FA_OPEN_ALWAYS | FA_WRITE);
  if (res != FR_OK) {
    usb_print("[STORAGE TASK] Failed opening log file \"%s\": %d\n", file_name,
             res);
    return res;
  }

  usb_print("[STORAGE TASK] Going to end of file\n");
  res = f_lseek(&EULER_LOG_FILE, f_size(&EULER_LOG_FILE));
  if (res != FR_OK) {
    usb_print("[STORAGE TASK] Failed going to end of file: %d\n", res);
    return res;
  }
  return res;
}

static int32_t format_log_str(char *str, log_elem_t *log_entry) {
  int32_t str_len = 0;
  switch (log_entry->log_type) {
    case SENSOR: {
      sensor_log_elem_t *sensor_log_entry = &(log_entry->u.sensor_log);
      sensor_type_e sensor_type = sensor_log_entry->sens_type;
      str_len =
          snprintf(str, SD_STRFMT_LEN, "%lu;%d;%hi,%d,", log_entry->ts, SENSOR,
                   sensor_log_entry->sensor_board_id, sensor_type);
      switch (sensor_type) {
        case BARO: {
          baro_data_t *baro_data_ptr = &(sensor_log_entry->sensor_data.baro);
          str_len += snprintf(str + str_len, SD_STRFMT_LEN, "%ld,%ld,%lu\n",
                              baro_data_ptr->pressure,
                              baro_data_ptr->temperature, baro_data_ptr->ts);
        } break;
        case IMU_1:
        case IMU_2: {
          imu_data_t *imu_data_ptr = &(sensor_log_entry->sensor_data.imu);
          str_len += snprintf(
              str + str_len, SD_STRFMT_LEN, "%hd,%hd,%hd,%hd,%hd,%hd,%lu\n",
              imu_data_ptr->acc_x, imu_data_ptr->acc_y, imu_data_ptr->acc_z,
              imu_data_ptr->gyro_x, imu_data_ptr->gyro_y, imu_data_ptr->gyro_z,
              imu_data_ptr->ts);
        } break;
        case GPS: {
          gps_data_t *gps_data = &(sensor_log_entry->sensor_data.gps);
          str_len += snprintf(
              str + str_len, SD_STRFMT_LEN,
              "%ld,%ld,%ld,%d,%ld,%d,%ld,%d,%hd,%hd\n", gps_data->hour,
              gps_data->minute, gps_data->second, gps_data->lat_deg,
              gps_data->lat_decimal, gps_data->lon_deg, gps_data->lon_decimal,
              gps_data->satellite, gps_data->altitude, gps_data->HDOP);
        } break;
        case BATTERY: {
          battery_data_t *battery_data = &(sensor_log_entry->sensor_data.bat);
          str_len += snprintf(str + str_len, SD_STRFMT_LEN, "%hd,%hd,%hd,%hd\n",
                              battery_data->battery, battery_data->consumption,
                              battery_data->current, battery_data->supply);
        } break;
        default:
          break;
      }
    } break;
    case STATE: {
      str_len = snprintf(str, SD_STRFMT_LEN, "%lu;%d;%d\n", log_entry->ts,
                         STATE, log_entry->u.state.flight_phase);
    } break;
    case ESTIMATOR_VAR_1D: {
      str_len =
          snprintf(str, SD_STRFMT_LEN, "%lu;%d;%ld,%ld,%ld\n", log_entry->ts,
                   ESTIMATOR_VAR_1D, log_entry->u.est_var.position_world[2],
                   log_entry->u.est_var.velocity_rocket[0],
                   log_entry->u.est_var.acceleration_rocket[0]);
    } break;
    case CONTROLLER_OUTPUT: {
      str_len =
          snprintf(str, SD_STRFMT_LEN, "%lu;%d;%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n", log_entry->ts,
                   CONTROLLER_OUTPUT, log_entry->u.cont_out.control_input,
				   log_entry->u.cont_out.reference_error,
				   log_entry->u.cont_out.integrated_error,
				   log_entry->u.cont_out.sf_ref_altitude_AGL,
				   log_entry->u.cont_out.sf_velocity,
				   log_entry->u.cont_out.ref_velocity,
				   log_entry->u.cont_out.tracking_feedback,
				   log_entry->u.cont_out.lowerboundary_aw,
				   log_entry->u.cont_out.upperboundary_aw,
				   log_entry->u.cont_out.exit_flag,
				   log_entry->u.cont_out.num_iterations);
    } break;
    case MOTOR_POSITION: {
      str_len = snprintf(str, SD_STRFMT_LEN, "%lu;%d;%ld,%ld\n", log_entry->ts,
                         MOTOR_POSITION, log_entry->u.motor.desired_position,
                         log_entry->u.motor.actual_position);
    } break;
    case MSG: {
      str_len = snprintf(str, SD_STRFMT_LEN, "%lu;%d;%s\n", log_entry->ts, MSG,
                         log_entry->u.msg);
    } break;
  }
  return str_len;
}
