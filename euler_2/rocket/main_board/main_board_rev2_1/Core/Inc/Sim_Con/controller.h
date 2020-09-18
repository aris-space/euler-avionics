//
// Created by Tun Kapgen on 05.06.20.
//

#ifndef C_IMPLEMENTATION_CONTROLLER_H
#define C_IMPLEMENTATION_CONTROLLER_H

#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "Sim_Con/flight_phase_detection.h"
#include "Sim_Con/controller_const.h"

/* function declaration */
void init_coeff(control_data_t *control_data);
void compute_control_input(control_data_t *control_data,
		flight_phase_detection_t *current_flight_phase_detection);
void control_data_reset(control_data_t *control_data);
void control_data_init(control_data_t *control_data);
void evaluate_polyfit(control_data_t *control_data);
void compute_antiwindup_boundaries(control_data_t *control_data);
void compute_reference_error(control_data_t *control_data);
void check_apogee_approach_phase(control_data_t *control_data,
		flight_phase_detection_t *flight_phase_detection);
void save_evaluated_polyfits_to_file(control_data_t *control_data);
void compute_test_control_input(control_data_t *control_data);

#endif //C_IMPLEMENTATION_CONTROLLER_H
