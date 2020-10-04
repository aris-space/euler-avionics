/*
 * Rs.h
 *
 * Created on: 20.09.2020
 *     Author: Imre Kertesz
 */

#ifndef ARIS_REED_SOLOMON_RS_H
#define ARIS_REED_SOLOMON_RS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Util/telemetry_util.h"

#define mm  8           /* RS code over GF(2**mm) */
#define nn  255         /* nn=2**mm -1   length of codeword */
#define tt  20          /* number of errors that can be corrected */
#define kk  (nn-(2*tt))        /* kk = nn-2*tt  - length of data without parity bits*/
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

void struct_to_poly(telemetry_t t_data, int16_t *data); //converts struct to polynomial form
telemetry_t poly_to_struct(int16_t const *recd); //converts polynomial back to struct
void compress_data(int16_t const *recd, int16_t *recd_compact); //compresses the data
int16_t *decompress_data(int16_t const *recd_compact); //decompresses the data
void print_look_up_table(int16_t const * alpha_to, int16_t const *index_of);

#endif //ARIS_REED_SOLOMON_RS_H
