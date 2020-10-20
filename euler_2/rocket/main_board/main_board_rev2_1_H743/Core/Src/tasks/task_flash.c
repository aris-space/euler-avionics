/*
 * task_flash.c
 *
 *  Created on: Aug 20, 2020
 *      Author: Nemanja
 */
#include "tasks/task_flash.h"
#include "drivers/flash/w25qxx.h"
#include "Util/util.h"
#include "Util/logging_util.h"
#include "Util/mutex.h"
#include "main.h"
#include <stdlib.h>
#include <stdio.h>

static inline log_entry_type_e pack_base(log_elem_t *log_elem,
                                        uint8_t *flash_buffer,
                                        uint16_t *flash_buffer_idx) {
  memcpy(&(flash_buffer[*flash_buffer_idx]), &log_elem->ts,
         sizeof(timestamp_t));
  *flash_buffer_idx += sizeof(timestamp_t);

  /* write log type */
  uint8_t log_type_uint8_t = (uint8_t)log_elem->log_type;
  memcpy(&(flash_buffer[*flash_buffer_idx]), &log_type_uint8_t,
         sizeof(uint8_t));
  *flash_buffer_idx += sizeof(uint8_t);

  return log_elem->log_type;
}

static inline void pack_sensor(sensor_log_elem_t *sensor_log,
                              uint8_t *flash_buffer,
                              uint16_t *flash_buffer_idx) {
  memcpy(&(flash_buffer[*flash_buffer_idx]), &(sensor_log->sensor_board_id),
         sizeof(board_id_t));
  *flash_buffer_idx += sizeof(board_id_t);

  uint8_t sensor_type_uint8_t = (uint8_t)sensor_log->sens_type;
  memcpy(&(flash_buffer[*flash_buffer_idx]), &sensor_type_uint8_t,
         sizeof(uint8_t));
  *flash_buffer_idx += sizeof(uint8_t);

  switch (sensor_log->sens_type) {
    case BARO: {
      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.baro.pressure), sizeof(int32_t));
      *flash_buffer_idx += sizeof(int32_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.baro.temperature), sizeof(int32_t));
      *flash_buffer_idx += sizeof(int32_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.baro.ts), sizeof(timestamp_t));
      *flash_buffer_idx += sizeof(timestamp_t);
    } break;
    case IMU_1:
    case IMU_2: {
      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.imu.gyro_x), sizeof(int16_t));
      *flash_buffer_idx += sizeof(int16_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.imu.gyro_y), sizeof(int16_t));
      *flash_buffer_idx += sizeof(int16_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.imu.gyro_z), sizeof(int16_t));
      *flash_buffer_idx += sizeof(int16_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.imu.acc_x), sizeof(int16_t));
      *flash_buffer_idx += sizeof(int16_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.imu.acc_y), sizeof(int16_t));
      *flash_buffer_idx += sizeof(int16_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.imu.acc_z), sizeof(int16_t));
      *flash_buffer_idx += sizeof(int16_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.imu.ts), sizeof(timestamp_t));
      *flash_buffer_idx += sizeof(timestamp_t);
    } break;
    case GPS: {
      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.gps.hour), sizeof(uint32_t));
      *flash_buffer_idx += sizeof(uint32_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.gps.minute), sizeof(uint32_t));
      *flash_buffer_idx += sizeof(uint32_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.gps.second), sizeof(uint32_t));
      *flash_buffer_idx += sizeof(uint32_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.gps.satellite), sizeof(uint8_t));
      *flash_buffer_idx += sizeof(uint8_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.gps.lat_deg), sizeof(uint8_t));
      *flash_buffer_idx += sizeof(uint8_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.gps.lat_decimal), sizeof(uint32_t));
      *flash_buffer_idx += sizeof(uint32_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.gps.lon_deg), sizeof(uint8_t));
      *flash_buffer_idx += sizeof(uint8_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.gps.lon_decimal), sizeof(uint32_t));
      *flash_buffer_idx += sizeof(uint32_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.gps.fix), sizeof(uint8_t));
      *flash_buffer_idx += sizeof(uint8_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.gps.HDOP), sizeof(uint16_t));
      *flash_buffer_idx += sizeof(uint16_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.gps.altitude), sizeof(uint16_t));
      *flash_buffer_idx += sizeof(uint16_t);
    } break;
    case BATTERY: {
      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.bat.supply), sizeof(uint16_t));
      *flash_buffer_idx += sizeof(uint16_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.bat.battery), sizeof(uint16_t));
      *flash_buffer_idx += sizeof(uint16_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.bat.current), sizeof(uint16_t));
      *flash_buffer_idx += sizeof(uint16_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.bat.consumption), sizeof(uint16_t));
      *flash_buffer_idx += sizeof(uint16_t);

      memcpy(&(flash_buffer[*flash_buffer_idx]),
             &(sensor_log->sensor_data.bat.power), sizeof(uint16_t));
      *flash_buffer_idx += sizeof(uint16_t);
    } break;
    default:
      usb_print("[FLASH-WRITE] Bad sensor type!\n");
      break;
  }
}

static inline void pack_state(flight_phase_detection_t *flight_phase,
                             uint8_t *flash_buffer,
                             uint16_t *flash_buffer_idx) {
  uint8_t flight_phase_uint8_t = (uint8_t)flight_phase->flight_phase;
  memcpy(&(flash_buffer[*flash_buffer_idx]), &flight_phase_uint8_t,
         sizeof(uint8_t));
  *flash_buffer_idx += sizeof(uint8_t);

  uint8_t mach_regime_uint8_t = (uint8_t)flight_phase->mach_regime;
  memcpy(&(flash_buffer[*flash_buffer_idx]), &mach_regime_uint8_t,
         sizeof(uint8_t));
  *flash_buffer_idx += sizeof(uint8_t);

  memcpy(&(flash_buffer[*flash_buffer_idx]), &(flight_phase->mach_number),
         sizeof(float));
  *flash_buffer_idx += sizeof(float);

  memcpy(&(flash_buffer[*flash_buffer_idx]),
         &(flight_phase->t_bias_reset_start), sizeof(int8_t));
  *flash_buffer_idx += sizeof(int8_t);
}

static inline void pack_estimator_var(state_est_data_t *state_est,
                                    uint8_t *flash_buffer,
                                    uint16_t *flash_buffer_idx) {
  memcpy(&(flash_buffer[*flash_buffer_idx]), &(state_est->position_world),
         3 * sizeof(int32_t));
  *flash_buffer_idx += 3 * sizeof(int32_t);

  memcpy(&(flash_buffer[*flash_buffer_idx]), &(state_est->velocity_rocket),
         3 * sizeof(int32_t));
  *flash_buffer_idx += 3 * sizeof(int32_t);

  memcpy(&(flash_buffer[*flash_buffer_idx]), &(state_est->acceleration_rocket),
         3 * sizeof(int32_t));
  *flash_buffer_idx += 3 * sizeof(int32_t);

  memcpy(&(flash_buffer[*flash_buffer_idx]), &(state_est->velocity_world),
         3 * sizeof(int32_t));
  *flash_buffer_idx += 3 * sizeof(int32_t);

  memcpy(&(flash_buffer[*flash_buffer_idx]), &(state_est->acceleration_world),
         3 * sizeof(int32_t));
  *flash_buffer_idx += 3 * sizeof(int32_t);
}

static inline void pack_controller_output(control_log_elem_t *cont_out,
                                        uint8_t *flash_buffer,
                                        uint16_t *flash_buffer_idx) {
  memcpy(&(flash_buffer[*flash_buffer_idx]), &(cont_out->control_input),
         sizeof(int32_t));
  *flash_buffer_idx += sizeof(int32_t);

  memcpy(&(flash_buffer[*flash_buffer_idx]), &(cont_out->reference_error),
         sizeof(int32_t));
  *flash_buffer_idx += sizeof(int32_t);

  memcpy(&(flash_buffer[*flash_buffer_idx]), &(cont_out->integrated_error),
         sizeof(int32_t));
  *flash_buffer_idx += sizeof(int32_t);
}

static inline void pack_motor_position(motor_log_elem_t *motor,
                                     uint8_t *flash_buffer,
                                     uint16_t *flash_buffer_idx) {
  memcpy(&(flash_buffer[*flash_buffer_idx]), &(motor->desired_position),
         sizeof(int32_t));
  *flash_buffer_idx += sizeof(int32_t);

  memcpy(&(flash_buffer[*flash_buffer_idx]), &(motor->actual_position),
         sizeof(int32_t));
  *flash_buffer_idx += sizeof(int32_t);
}

static inline void pack_msg(char *msg, uint8_t *flash_buffer,
                           uint16_t *flash_buffer_idx) {
  /* insert string length before string itself -- limit 255 chars + null
   * terminator */
  uint8_t string_length = (uint8_t)strlen(msg) + 1;

  memcpy(&(flash_buffer[*flash_buffer_idx]), &string_length, sizeof(uint8_t));
  *flash_buffer_idx += sizeof(uint8_t);

  memcpy(&(flash_buffer[*flash_buffer_idx]), msg, string_length);
  *flash_buffer_idx += string_length;
}

static inline void packValue(log_elem_t *log_elem, uint8_t *flash_buffer,
                             uint16_t *flash_buffer_idx) {
  log_entry_type_e log_type =
      pack_base(log_elem, flash_buffer, flash_buffer_idx);
  switch (log_type) {
    case SENSOR: {
      pack_sensor(&log_elem->u.sensor_log, flash_buffer, flash_buffer_idx);
    } break;
    case STATE: {
      pack_state(&log_elem->u.state, flash_buffer, flash_buffer_idx);
    } break;
    case ESTIMATOR_VAR_1D:
    case ESTIMATOR_VAR_3D: {
      pack_estimator_var(&log_elem->u.est_var, flash_buffer, flash_buffer_idx);
    } break;
    case CONTROLLER_OUTPUT: {
      pack_controller_output(&log_elem->u.cont_out, flash_buffer,
                           flash_buffer_idx);
    } break;
    case MOTOR_POSITION: {
      pack_motor_position(&log_elem->u.motor, flash_buffer, flash_buffer_idx);
    } break;
    case MSG: {
      pack_msg(log_elem->u.msg, flash_buffer, flash_buffer_idx);
    } break;
    default:
      usb_print("[FLASH-WRITE] Log type not recognized!\n");
      break;
  }
}

static inline void write_value(log_elem_t *log_elem, uint8_t *flash_buffer,
                              uint16_t *flash_buffer_idx) {
  packValue(log_elem, flash_buffer, flash_buffer_idx);
}

#ifdef FLASH_READ
static inline void read_value(uint8_t *flash_read_buffer,
                             uint16_t *flash_read_buffer_idx,
                             log_elem_t *log_elem, uint16_t sector_id) {
  static char print_buf[200] = {0};
  timestamp_t ts;

  memcpy(&ts, &flash_read_buffer[*flash_read_buffer_idx], sizeof(timestamp_t));
  *flash_read_buffer_idx += sizeof(timestamp_t);

  uint8_t log_type_uint8_t = flash_read_buffer[(*flash_read_buffer_idx)++];
  log_entry_type_e log_type = (log_entry_type_e)log_type_uint8_t;
  switch (log_type) {
    case SENSOR: {
      board_id_t sensor_board_id;
      memcpy(&sensor_board_id, &flash_read_buffer[*flash_read_buffer_idx],
             sizeof(board_id_t));
      *flash_read_buffer_idx += sizeof(board_id_t);

      uint8_t sensor_type_uint8_t;
      memcpy(&sensor_type_uint8_t, &flash_read_buffer[*flash_read_buffer_idx],
             sizeof(uint8_t));
      *flash_read_buffer_idx += sizeof(uint8_t);

      sensor_type_e sens_type = (sensor_type_e)sensor_type_uint8_t;

      snprintf(print_buf + strlen(print_buf), 200,
               "[FLASH]: Sector#: %hu, Page#: %hu; Data: %lu;%d;%hi,%d | "
               "Sensor data: ",
               sector_id, *flash_read_buffer_idx / 256, ts, SENSOR,
               sensor_board_id, sens_type);

      switch (sens_type) {
        case BARO: {
          baro_data_t baro;

          memcpy(&baro.pressure, &(flash_read_buffer[*flash_read_buffer_idx]),
                 sizeof(int32_t));
          *flash_read_buffer_idx += sizeof(int32_t);

          memcpy(&baro.temperature,
                 &(flash_read_buffer[*flash_read_buffer_idx]), sizeof(int32_t));
          *flash_read_buffer_idx += sizeof(int32_t);

          memcpy(&baro.ts, &(flash_read_buffer[*flash_read_buffer_idx]),
                 sizeof(timestamp_t));
          *flash_read_buffer_idx += sizeof(timestamp_t);

          snprintf(print_buf + strlen(print_buf), 200,
                   "P: %ld,T: %ld,Ts: %lu\n", baro.pressure, baro.temperature,
                   baro.ts);
        } break;

        case IMU: {
          imu_data_t imu;

          memcpy(&imu.gyro_x, &(flash_read_buffer[*flash_read_buffer_idx]),
                 sizeof(int16_t));
          *flash_read_buffer_idx += sizeof(int16_t);

          memcpy(&imu.gyro_y, &(flash_read_buffer[*flash_read_buffer_idx]),
                 sizeof(int16_t));
          *flash_read_buffer_idx += sizeof(int16_t);

          memcpy(&imu.gyro_z, &(flash_read_buffer[*flash_read_buffer_idx]),
                 sizeof(int16_t));
          *flash_read_buffer_idx += sizeof(int16_t);

          memcpy(&imu.acc_x, &(flash_read_buffer[*flash_read_buffer_idx]),
                 sizeof(int16_t));
          *flash_read_buffer_idx += sizeof(int16_t);

          memcpy(&imu.acc_y, &(flash_read_buffer[*flash_read_buffer_idx]),
                 sizeof(int16_t));
          *flash_read_buffer_idx += sizeof(int16_t);

          memcpy(&imu.acc_z, &(flash_read_buffer[*flash_read_buffer_idx]),
                 sizeof(int16_t));
          *flash_read_buffer_idx += sizeof(int16_t);

          memcpy(&imu.ts, &(flash_read_buffer[*flash_read_buffer_idx]),
                 sizeof(timestamp_t));
          *flash_read_buffer_idx += sizeof(timestamp_t);

          snprintf(
              print_buf + strlen(print_buf), 200,
              "Ax: %hd, Ay: %hd, Az: %hd,Gx: %hd,Gy: %hd,Gz: %hd,Ts: %lu\n",
              imu.acc_x, imu.acc_y, imu.acc_z, imu.gyro_x, imu.gyro_y,
              imu.gyro_z, imu.ts);
        } break;

        case GPS: {
          gps_data_t gps;
          memcpy(&gps, &flash_read_buffer[*flash_read_buffer_idx], sizeof(gps));
          //					snprintf(print_buf +
          // strlen(print_buf),
          // 200, 							"P: %ld,T: %ld,Ts:
          // %lu\n", baro.pressure, baro.temperature, baro.ts);
          *flash_read_buffer_idx += sizeof(gps);
        } break;

        case BATTERY: {
          battery_data_t bat;
          memcpy(&bat, &flash_read_buffer[*flash_read_buffer_idx], sizeof(bat));
          snprintf(print_buf + strlen(print_buf), 200,
                   "BAT: %hd,CONS: %hd,CURR: %hd,SUPPLY: %hd\n", bat.battery,
                   bat.consumption, bat.current, bat.supply);
          *flash_read_buffer_idx += sizeof(bat);
        } break;
        default:
          usb_print("[FLASH-READ] Bad sensor type! idx: %hu\n",
                   *flash_read_buffer_idx);
          break;
      }
      // UsbPrint("Sensor...\n");
    } break;
    case MSG: {
      // UsbPrint("Msg...\n");
    } break;
    default:
      usb_print("[FLASH-READ] Bad log type! idx: %hu\n", *flash_read_buffer_idx);
      break;
  }
  usb_print(print_buf);
  osDelay(20);
  print_buf[0] = 0;
}
#endif

/* TODO [nemanja]: move initialization to a separate task */
static inline void init_flash() {
  //	[FLASH] Page Size: 256 Bytes
  //	[FLASH] Page Count: 65536
  //	[FLASH] Sector Size: 4096 Bytes
  //	[FLASH] Sector Count: 4096
  //	[FLASH] Block Size: 65536 Bytes
  //	[FLASH] Block Count: 256
  //	[FLASH] Capacity: 16384 KiloBytes
  usb_print("[FLASH] Init Begin...\n");
  W25qxx_Init();
  usb_print("[FLASH] Page Size: %d Bytes\n", w25qxx.PageSize);
  osDelay(1);
  usb_print("[FLASH] Page Count: %d\n", w25qxx.PageCount);
  osDelay(1);
  usb_print("[FLASH] Sector Size: %d Bytes\n", w25qxx.SectorSize);
  osDelay(1);
  usb_print("[FLASH] Sector Count: %d\n", w25qxx.SectorCount);
  osDelay(1);
  usb_print("[FLASH] Block Size: %d Bytes\n", w25qxx.BlockSize);
  osDelay(1);
  usb_print("[FLASH] Block Count: %d\n", w25qxx.BlockCount);
  osDelay(1);
  usb_print("[FLASH] Capacity: %d KiloBytes\n", w25qxx.CapacityInKiloByte);
  osDelay(1);
  usb_print("[FLASH] Init Done\n");
  osDelay(1);
  usb_print("[FLASH] Erasing data on chip\n");
  W25qxx_EraseChip();
  usb_print("[FLASH] Data on chip erased!\n");
  osDelay(15000);
}

#ifdef FLASH_READ
static int compare_arrays(uint8_t a[], uint8_t b[], int n) {
  int ii = -1;
  for (ii = 0; ii < n; ii++) {
    if (a[ii] != b[ii]) break;
  }
  if (ii != n) {
    usb_print("[FLASH] ARRAYS DIFFERENT %d!\n", ii);
    return 1;
  } else {
    usb_print("[FLASH] ARRAYS SAME!\n");
    return 0;
  }
}
#endif

uint8_t flash_buffer[FLASH_BUFFER_LEN];

void vTaskFlash(void *argument) {
  osDelay(500);

  //uint8_t *flash_buffer = (uint8_t *)calloc(FLASH_BUFFER_LEN, sizeof(uint8_t));
  memset(flash_buffer, 0, FLASH_BUFFER_LEN);
  uint16_t flash_buffer_idx = 0;

#ifdef FLASH_READ
  uint8_t *flash_read_buffer =
      (uint8_t *)calloc(FLASH_BUFFER_LEN, sizeof(uint8_t));
  uint16_t flash_read_buffer_idx = 0;
#endif

  init_flash();

  uint16_t sector_id = 0;

  while (1) {
    log_elem_t curr_log_elem = {0};

    while (flash_buffer_idx < (FLASH_BUFFER_LEN - sizeof(log_elem_t))) {
      if (osMessageQueueGet(flash_queue, &curr_log_elem, NULL, osWaitForever) ==
          osOK) {
        // UsbPrint("Queue size:%d", osMessageQueueGetCount(logf_queue));
        write_value(&curr_log_elem, flash_buffer, &flash_buffer_idx);
      }
    }
    /* reset log buffer index */
    flash_buffer_idx = 0;
    usb_print("[FLASH] flash_buffer filled.\n");
    HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);

    W25qxx_WriteSector(flash_buffer, sector_id, 0, FLASH_BUFFER_LEN);

#ifdef FLASH_READ
    osDelay(500);
    W25qxx_ReadSector(flash_read_buffer, sector_id, 0, FLASH_BUFFER_LEN);
    // osDelay(1000);

    if (compare_arrays(flash_buffer, flash_read_buffer, FLASH_BUFFER_LEN)) {
      osDelay(100);
      usb_print("[FLASH] Re-reading the data\n");
      osDelay(100);
      W25qxx_ReadSector(flash_read_buffer, sector_id, 0, FLASH_BUFFER_LEN);
    }

    compare_arrays(flash_buffer, flash_read_buffer, FLASH_BUFFER_LEN);

    flash_read_buffer_idx = 0;
    while (flash_read_buffer_idx < (FLASH_BUFFER_LEN - sizeof(log_elem_t))) {
      read_value(flash_read_buffer, &flash_read_buffer_idx, &curr_log_elem,
                sector_id);
    }
#endif

    HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
    usb_print("[FLASH] Sector %hu filled.\n", sector_id);

    if (sector_id == 4095) {
      sector_id = 0;
      usb_print("[FLASH] All sectors written!\n");
      break; /* end the task */
    } else {
      sector_id++;
    }
  }
  //free(flash_buffer);
#ifdef FLASH_READ
  free(flash_read_buffer);
#endif
}
