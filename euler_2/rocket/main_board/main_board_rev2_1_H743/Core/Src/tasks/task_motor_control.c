/*
 * task_motor_control.c
 *
 *  Created on: May 23, 2020
 *      Author: Jonas
 */

#include "util/logging_util.h"
#include "tasks/task_motor_control.h"
#include "drivers/epos4/epos4.h"

static void test_airbrakes(int32_t position);

void vTaskMotorCont(void *argument) {
  /* For periodic update */
  uint32_t tick_count, tick_update;

  osStatus_t motor_status = osOK;

  /* Telemetry Command and FSM State */
  command_e telemetry_command = IDLE_COMMAND;
  flight_phase_detection_t flight_phase_detection = {0};
  flight_phase_detection.flight_phase = IDLE;
  flight_phase_detection.mach_number = SUBSONIC;

  /* Initialisation */
   int8_t position_mode = 0x08;
  /* Profile Position Mode */
  //int8_t position_mode = 0x01;
  int32_t PPM_velocity = 10000;
  int32_t PPM_acceleration = 100000;
  int32_t PPM_deceleration = 100000;

  osDelay(3000);

  /* Controller Variables */
  int32_t controller_actuation = 0;

  /* Task Variables */
  int32_t desired_motor_position = 0;
  int32_t measured_motor_position = 0;

  /* Enable Motor */
  while (enable_motor() != osOK) {
    osDelay(1000);
  };

  /* Set Position Mode */
  set_position_mode(position_mode);
  while (set_position_mode(position_mode) != osOK) {
    osDelay(1000);
  };

  if (position_mode == 0x01) {
    motor_status =
        configure_ppm(PPM_velocity, PPM_acceleration, PPM_deceleration);
  }

  /* Infinite loop */
  tick_count = osKernelGetTickCount();
  tick_update = osKernelGetTickFreq() / MOTOR_TASK_FREQUENCY;

  while (1) {
    tick_count += tick_update;

    /* Read Telemetry Command */
    read_mutex(&command_mutex, &global_telemetry_command, &telemetry_command,
              sizeof(global_telemetry_command));

    /* Read FSM State */
    read_mutex(&fsm_mutex, &global_flight_phase_detection,
              &flight_phase_detection, sizeof(global_flight_phase_detection));

    /* Read in Current Controller Output */
    read_mutex(&controller_mutex, &controller_output_global,
              &controller_actuation, sizeof(controller_actuation));

    /* Transform 0-1 Controller output to controller output of motor */
    desired_motor_position =
        (int32_t)(((float)controller_actuation) / 1000 * (-150));

    /* Check Bounds */
    if (desired_motor_position > 2) {
      desired_motor_position = 2;
    }

    if (desired_motor_position < -150) {
      desired_motor_position = -150;
    }

    /* If we are in IDLE, THRUSTING or DESCENDING
     * the Motor is not allowed to Move!
     */
    if (flight_phase_detection.flight_phase == COASTING) {
      /* Move the Motor */
      motor_status = move_to_position(desired_motor_position);
    } else {
      motor_status = move_to_position(0);
    }

    if(osKernelGetTickCount() < 10000 && osKernelGetTickCount() > 9960){
    	test_airbrakes(-100);
    }

    /* Airbrake Test if telemetry command is given and we are in idle state */
    if (flight_phase_detection.flight_phase == IDLE &&
        telemetry_command == AIRBRAKE_TEST_COMMAND &&
        osKernelGetTickCount() < 60000) {
      test_airbrakes(-140);
      telemetry_command = IDLE_COMMAND;
    }

    /* Read Current Motor Position */
    motor_status = get_position(&measured_motor_position);

    /* Transform Motor Position to a value between [0-1000] */
    if(measured_motor_position < 0){
    	measured_motor_position = 0;
    }
    else{
    	measured_motor_position = measured_motor_position * 1000 / (-150);
    }

    /* Write To global airbrake extension */
    if (acquire_mutex(&airbrake_ext_mutex) == osOK) {
      global_airbrake_ext_meas = measured_motor_position;
      release_mutex(&airbrake_ext_mutex);
    }

    //usb_print("[MOTOR] Read Position:%d\n", measured_motor_position);

    /* Log Motor Position and Desired Motor Position */
    log_motor(osKernelGetTickCount(), desired_motor_position,
             measured_motor_position);

    if (motor_status != osOK && flight_phase_detection.flight_phase == IDLE) {
      disable_motor();
      osDelay(1000);
      enable_motor();
    }

    osDelayUntil(tick_count);
  }
}

static void test_airbrakes(int32_t position) {
  move_to_position(position);
  osDelay(100);
  move_to_position(2);
}
