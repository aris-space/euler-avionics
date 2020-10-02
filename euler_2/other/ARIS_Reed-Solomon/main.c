/*
 * main.c
 *
 * Created on: 20.09.2020
 *     Author: Imre Kertesz
 */

#include <stdio.h>
#include "RS/RS.h"

int main() {

    telemetry_t t_data;
    t_data.airbrake_extension = 5;
    t_data.height = 2500;
    t_data.velocity = 4000;

    //gps
    t_data.gps.HDOP = 1;
    t_data.gps.altitude = 2550;
    t_data.gps.fix = 6;
    t_data.gps.hour = 12;
    t_data.gps.lat_decimal = 36;
    t_data.gps.lat_deg = 123;
    t_data.gps.lon_decimal = -33;
    t_data.gps.lon_deg = 121;
    t_data.gps.satellite = 4;
    t_data.gps.minute = 22;
    t_data.gps.second = 16;

    //sensor board
    t_data.sb_data.acc_x = 56;
    t_data.sb_data.acc_y = -55;
    t_data.sb_data.acc_z = 76;
    t_data.sb_data.gyro_x = 43;
    t_data.sb_data.gyro_y = 25;
    t_data.sb_data.gyro_z = 15;
    t_data.sb_data.pressure = 5566;
    t_data.sb_data.temperature = 6733;

    //battery
    t_data.battery.battery = 12;
    t_data.battery.consumption = 120;
    t_data.battery.current = 230;

    //flight phase
    t_data.flight_phase = COASTING;

    print_struct(&t_data);

    /*=========================Initialization Part================================*/
    int16_t alpha_to[nn + 1];
    int16_t index_of[nn + 1];
    int16_t gg[nn - kk + 1];
    int16_t pp[mm + 1];
    int16_t recd[nn];
    int16_t bb[nn - kk];
    int16_t data[kk];
    int16_t recd_compact[nn-kk+kk2];

    printf("size of telemetry struct: %llu bytes\n", sizeof(t_data));
    printf("size of data: %llu bytes\n", sizeof(data));

    register int i;

    // convert struct to coefficients of polynomial
    struct_to_poly(t_data, data);

    // make a copy of the data before it gets corrupted on purpose
    uint16_t orig_data[kk];
    for(i=0;i<kk;i++){
        orig_data[i] = data[i];
    }
    //set irreducible polynomial
    set_irr_poly(pp);


/* generate the Galois Field GF(2**mm) */
    generate_gf(pp, index_of, alpha_to);

    //print_look_up_table(alpha_to, index_of);

/* compute the generator polynomial for this RS code */
    gen_poly(gg, alpha_to, index_of);

/*====================================Encoding Part=========================================*/
/* encode data[] to produce parity in bb[].  Data input and parity output
   is in polynomial form
*/
    encode_rs(bb, index_of, alpha_to, gg, data);

/* put the transmitted codeword, made up of data plus parity, in recd[] */
    for (i = 0; i < nn - kk; i++) recd[i] = bb[i];

    //corrupt the data by shifting the last 110 bytes by one
    for(i=35;i<kk-1;i++){
        data[i]=data[i+1];
    }
    //and change some values at random places
    data[1] = 5;
    data[110] = 10;

    //copy data into recd
    for (i = 0; i < kk; i++) recd[i + nn - kk] = data[i];

    //compress data for transmission
    compress_data(recd, recd_compact);

    //recd_compact can now be sent to the Xbee


    printf("----------------------------------------\n");
    for (i=0;i<nn-kk;i++){
        printf("%i  %02X | %02X\n", i, recd[i], recd_compact[i]);
    }
    for (i=nn-kk; i<nn-kk+kk2;i++){
        printf("%i  %02X %02X  | %02X\n",i, recd[2*(i-nn+kk)+nn-kk], recd[2*(i-nn+kk)+nn-kk+1], recd_compact[i]);
    }




/*==================================Decoding Part========================================*/
    //decompress data
    int16_t *received = decompress_data(recd_compact);
    for (i = 0; i < nn; i++)
        received[i] = index_of[received[i]];          /* put recd[i] into index form */

/* decode recv[] */
    decode_rs(received, alpha_to, index_of);         /* recd[] is returned in polynomial form */

/* print out the relevant stuff - initial and decoded {parity and message} */
/*
    printf("Results for Reed-Solomon code (n=%3d, k=%3d, t= %3d)\n\n", nn, kk, tt);
    printf("  i  data[i] corrupted data[i]   recd[i](decoded)   (data, recd in polynomial form)\n");
    for (i = 0; i < nn - kk; i++)
        printf("%3d    %3d      %3d\n", i, bb[i], recd[i]);
    for (i = nn - kk; i < nn; i++)
        printf("%3d    %3d      %3d\n", i, data[i - nn + kk], recd[i]);
*/
    printf("Results for Reed-Solomon code (n=%3d, k=%3d, t= %3d)\n\n", nn, kk, tt);
    printf("i  data[i](original)  data[i](corrupted)   recd[i](decoded)\n");
    for(i=0; i < kk; i++){
        printf("%d         %X                   %X                    %X\n", i, orig_data[i], data[i], received[nn-kk+i]);
    }

    //convert received polynomial back to struct
    telemetry_t rec = poly_to_struct(received);
    print_struct(&rec);

    return 0;
}
