/*
 * task_sens_read.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "util/logging_util.h"
#include "tasks/task_sens_read.h"

static void read_data_sb(sb_data_t *sb1, sb_data_t *sb2, sb_data_t *sb3);
#ifdef USB_DATA_ENABLE
static void read_data_usb();
#endif
static uint8_t calculate_checksum_sb(sb_data_t *sb_data);

/* SPI Read Data */
sb_data_t sb1_data = {0};
sb_data_t sb2_data = {0};
sb_data_t sb3_data = {0};

void vTaskSensRead(void *argument) {
  /* For periodic update */
  uint32_t tick_count, tick_update;

  osDelay(800);

  /* Infinite loop */
  tick_count = osKernelGetTickCount();
  tick_update = osKernelGetTickFreq() / SENSOR_READ_FREQUENCY;

  while (1) {
    tick_count += tick_update;

    /* Get Data */
#ifdef USB_DATA_ENABLE
      read_data_usb();
#else
      read_data_sb(&sb1_data, &sb2_data, &sb3_data);
#endif

    /* Log Data */
    log_sensor(tick_count, 1, BARO, &sb1_data.baro);
    log_sensor(tick_count, 1, IMU_1, &sb1_data.imu_1);
    log_sensor(tick_count, 1, IMU_2, &sb1_data.imu_2);
    log_sensor(tick_count, 2, BARO, &sb2_data.baro);
    log_sensor(tick_count, 2, IMU_1, &sb2_data.imu_1);
    log_sensor(tick_count, 2, IMU_2, &sb2_data.imu_2);
    log_sensor(tick_count, 3, BARO, &sb3_data.baro);
    log_sensor(tick_count, 3, IMU_1, &sb3_data.imu_1);
    log_sensor(tick_count, 3, IMU_2, &sb3_data.imu_2);

    /* Get new Data over SPI */

    HAL_SPI_Receive_DMA(&hspi1, (uint8_t *)&sb1_data, sizeof(sb1_data));
    HAL_SPI_Receive_DMA(&hspi2, (uint8_t *)&sb2_data, sizeof(sb2_data));
    HAL_SPI_Receive_DMA(&hspi3, (uint8_t *)&sb3_data, sizeof(sb3_data));

    /* Periodic Update */
    osDelayUntil(tick_count);
  }
}

/* SPI callback */
// void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
//	//HAL_SPIEx_FlushRxFifo(hspi);
//}


/* Read Data from Sensor Boards */
static void read_data_sb(sb_data_t *sb1, sb_data_t *sb2, sb_data_t *sb3) {
  /* Read SB 1, Write SB 1 Global Variable */
  uint8_t checksum;
  checksum = calculate_checksum_sb(sb1);
  if (checksum == sb1->checksum) {
    if (acquire_mutex(&sb1_mutex) == osOK) {
      sb1_global = *sb1;
      release_mutex(&sb1_mutex);
    }else{
    	HAL_SPI_DMAStop(&hspi1);
    	osDelay(1);
    	HAL_SPI_DMAResume(&hspi1);
    }
  }

  /* Read SB 2, Write SB 2 Global Variable  */
  checksum = calculate_checksum_sb(sb2);
  if (checksum == sb2->checksum) {
    if (acquire_mutex(&sb2_mutex) == osOK) {
      sb2_global = *sb2;
      release_mutex(&sb2_mutex);
    }
  }else{
  	HAL_SPI_DMAStop(&hspi2);
  	osDelay(1);
  	HAL_SPI_DMAResume(&hspi2);
  }

  /* Read SB 3, Write SB 3 Global Variable  */
  checksum = calculate_checksum_sb(sb3);
  if (checksum == sb3->checksum) {
    if (acquire_mutex(&sb3_mutex) == osOK) {
      sb3_global = *sb3;
      release_mutex(&sb3_mutex);
    }
  }else{
  	HAL_SPI_DMAStop(&hspi3);
  	osDelay(1);
  	HAL_SPI_DMAResume(&hspi3);
  }
}

/* Read Data from USB */
#ifdef USB_DATA_ENABLE
static void read_data_usb() {
  if (osMutexAcquire(usb_data_mutex.mutex, 10)) {
//    sscanf(usb_data_buffer,
//           "%ld,%ld,%ld;%hd,%hd,%hd,%hd,%hd,%hd,%ld|%ld,%ld,%ld;%hd,%hd,%hd,%"
//           "hd,%hd,%hd,%ld|%ld,%ld,%ld;%hd,%hd,%hd,%hd,%hd,%hd,%ld\n",
//           &sb1_baro.pressure, &sb1_baro.temperature, &sb1_baro.ts,
//           &sb1_imu.gyro_x, &sb1_imu.gyro_y, &sb1_imu.gyro_z, &sb1_imu.acc_x,
//           &sb1_imu.acc_y, &sb1_imu.acc_z, &sb1_imu.ts, &sb2_baro.pressure,
//           &sb2_baro.temperature, &sb2_baro.ts, &sb2_imu.gyro_x,
//           &sb2_imu.gyro_y, &sb2_imu.gyro_z, &sb2_imu.acc_x, &sb2_imu.acc_y,
//           &sb2_imu.acc_z, &sb2_imu.ts, &sb3_baro.pressure,
//           &sb3_baro.temperature, &sb3_baro.ts, &sb3_imu.gyro_x,
//           &sb3_imu.gyro_y, &sb3_imu.gyro_z, &sb3_imu.acc_x, &sb3_imu.acc_y,
//           &sb3_imu.acc_z, &sb3_imu.ts);
    osMutexRelease(usb_data_mutex.mutex);
  }
}
#endif

static uint8_t calculate_checksum_sb(sb_data_t *sb_data) {
  return sb_data->baro.pressure + sb_data->baro.temperature +
         sb_data->imu_1.gyro_x + sb_data->imu_1.gyro_y + sb_data->imu_1.gyro_z +
         sb_data->imu_1.acc_x + sb_data->imu_1.acc_y + sb_data->imu_1.acc_z +
		 sb_data->imu_2.gyro_x + sb_data->imu_2.gyro_y + sb_data->imu_2.gyro_z +
		 sb_data->imu_2.acc_x + sb_data->imu_2.acc_y + sb_data->imu_2.acc_z;
}


