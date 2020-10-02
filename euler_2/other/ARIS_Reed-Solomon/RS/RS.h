//
// Created by imrek on 20.09.2020.
//

#ifndef ARIS_REED_SOLOMON_RS_H
#define ARIS_REED_SOLOMON_RS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define mm  8           /* RS code over GF(2**mm) */
#define nn  255         /* nn=2**mm -1   length of codeword */
#define tt  55          /* number of errors that can be corrected */
#define kk  145         /* kk = nn-2*tt  - length of data without parity bits*/
#define kk2 (int)ceil((double)kk/2) /* length of compressed data without parity bits*/

/* sets the irreducible polynomial */
void set_irr_poly(int16_t *pp);

/* generates the Galois field */
void generate_gf(int16_t const *pp, int16_t *index_of, int16_t *alpha_to);

/* creates the generator polynomial */
void gen_poly(int16_t *gg, int16_t const *alpha_to, int16_t const *index_of);

/* generates the parity bits (data must be in polynomial form) */
void encode_rs(int16_t *bb, int16_t const *index_of, int16_t const *alpha_to, int16_t const *gg, int16_t const *data);

/* decodes the received data */
void decode_rs(int16_t * recd, int16_t const *alpha_to, int16_t const *index_of);

#pragma pack(push, 1)
/* Rocket state */
typedef enum {
    IDLE = 1,
    AIRBRAKE_TEST,
    THRUSTING,
    COASTING,
    DESCENT,
    RECOVERY
} flight_phase_e;

/* GPS data */
typedef struct {
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
    uint32_t lat_decimal;
    uint32_t lon_decimal;
    uint16_t HDOP;
    uint16_t altitude;
    uint8_t satellite;
    uint8_t lat_deg;
    uint8_t lon_deg;
    uint8_t fix;
} gps_data_t;

/* Battery Data */
typedef struct {
    uint16_t battery;
    uint16_t current;
    uint16_t consumption;
} telemetry_battery_data_t;

/* SB Data */
typedef struct {
    int32_t pressure;
    int32_t temperature;
    int16_t gyro_x, gyro_y, gyro_z;
    int16_t acc_x, acc_y, acc_z;
} telemetry_sb_data_t;

typedef struct {
    telemetry_sb_data_t sb_data;
    telemetry_battery_data_t battery;
    gps_data_t gps;
    int32_t height;
    int32_t velocity;
    int32_t airbrake_extension;
    flight_phase_e flight_phase;
} telemetry_t;
#pragma pack(pop)

void print_struct(telemetry_t* data_t); //prints out the struct in a readable way
void struct_to_poly(telemetry_t t_data, int16_t *data); //converts struct to polynomial form
telemetry_t poly_to_struct(int16_t const *recd); //converts polynomial back to struct
void compress_data(int16_t const *recd, int16_t *recd_compact); //compresses the data
int16_t *decompress_data(int16_t const *recd_compact); //decompresses the data
void print_look_up_table(int16_t const * alpha_to, int16_t const *index_of);

#endif //ARIS_REED_SOLOMON_RS_H
