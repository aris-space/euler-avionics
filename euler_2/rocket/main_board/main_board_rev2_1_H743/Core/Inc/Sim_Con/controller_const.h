//
// Created by Tun Kapgen on 09.06.20.
//
#ifndef C_IMPLEMENTATION_CONTROLLER_CONST_H
#define C_IMPLEMENTATION_CONTROLLER_CONST_H

/* Constants */
#include "controller.h"
#include <math.h>

#define LQR_ACTIVE true                                       //
#define CONTROLLER_SAMPLING_FREQ 100                          // Hz
#define POLY_DEG 30                                           // NEEDS CHANGE
#define OPT_TRAJ_CONTROL_INPUT 0.495415061477727              // -
#define CONTROL_ACTIVATION_MACH_NUMBER 0.55                   // -
#define CONTROL_DEACTIVATION_MACH_NUMBER 0.1                  // -
#define CONTROL_DEACTIVATION_ALTITUDE_AGL 1382.1581114217445  // NEEDS CHANGE
#define MIN_BOUNDARAY_AW 0.5                                  // m
#define NUM_POLYFITS 4                                        // -
#define NUM_GAINS NUM_POLYFITS - 1                            // -
#define M_AW 0.005                                            // -
#define SAFETY_COUNTER_THRESHOLD \
  (int)(fmaxf(ceil(CONTROLLER_SAMPLING_FREQ / 20.0), 2))  // -
#define DELTA_T 1.0f / CONTROLLER_SAMPLING_FREQ           // s
#define TARGET_AGOGEE 1435.0                              // m
#define TEST_CONTROLLER_USE_VELOCITY false                // -
#define TEST_CONTROLLER_MAX_VELOCITY 3.0f                 // m/s
#define TEST_CONTROLLER_MAX_ALTITUDE 20.0f                // m

/* Types */
typedef struct {
  float control_input;
  float reference_error;
  float integrated_error;

  float sf_ref_altitude_AGL;
  float sf_velocity;
  float ref_velocity;

  float lowerboundary_aw;
  float upperboundary_aw;

  uint16_t safety_counter;
  bool apogee_approach_phase;

  double gains[NUM_POLYFITS - 1];
  double poly_coeff[NUM_POLYFITS][POLY_DEG + 1];

} control_data_t;

#endif  // C_IMPLEMENTATION_CONTROLLER_CONST_H
