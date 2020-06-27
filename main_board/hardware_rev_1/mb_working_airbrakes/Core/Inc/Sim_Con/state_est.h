#include "Util/math_utils.h"
#include "Sim_Con/env.h"
#include "Sim_Con/kf.h"
#include "Sim_Con/flight_phase_detection.h"
#include "Sim_Con/state_est_settings.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#ifndef STATE_EST_H_
#define STATE_EST_H_

typedef struct {
    int32_t memory_length;
    float timestamps[MAX_LENGTH_ROLLING_MEMORY];
    float measurements[MAX_LENGTH_ROLLING_MEMORY];
    float noise_stdev;
    double polyfit_coeffs[EXTRAPOLATION_POLYFIT_DEGREE+1]; /* array size needs to be the degree of the polyfit plus 1 */
} extrapolation_rolling_memory_t;

static const extrapolation_rolling_memory_t EMPTY_MEMORY = { 0 };

void update_state_est_data(state_est_data_t *state_est_data, kf_state_t *kf_state);

void process_measurements(timestamp_t t, kf_state_t *kf_state, state_est_meas_t *state_est_meas, state_est_meas_t *state_est_meas_prior,
                          env_t *env, extrapolation_rolling_memory_t *extrapolation_rolling_memory);

void select_noise_models(kf_state_t *kf_state, flight_phase_detection_t *flight_phase_detection, env_t *env,
                         extrapolation_rolling_memory_t *extrapolation_rolling_memory);

void sensor_elimination_by_stdev(int32_t n, float measurements[n], bool measurement_active[n]);
void sensor_elimination_by_extrapolation(timestamp_t t, int32_t n, float measurements[n], bool measurement_active[n],
                                         extrapolation_rolling_memory_t *extrapolation_rolling_memory);

#endif
