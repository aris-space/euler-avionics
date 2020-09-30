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
    t_data.sb_data.acc_y = 55;
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

    printf("size of telemetry struct: %llu bytes\n", sizeof(t_data));
    printf("size of data: %llu bytes\n", sizeof(data));

    register int i;

    // convert struct to coefficients of polynomial
    struct_to_poly(t_data);

    // make a copy of the data before it gets corrupted on purpose
    uint8_t orig_data[kk];
    for(i=0;i<kk;i++){
        orig_data[i] = data[i];
    }

    set_irr_poly();


/* generate the Galois Field GF(2**mm) */
    generate_gf();
    /*
    printf("Look-up tables for GF(2**%2d)\n", mm);
    printf("  i   alpha_to[i]  index_of[i]\n");
    for (i = 0; i <= nn; i++)
        printf("%3d      %3d          %3d\n", i, alpha_to[i], index_of[i]);
    printf("\n\n");*/

/* compute the generator polynomial for this RS code */
    gen_poly();


/* for known data, stick a few numbers into a zero codeword. Data is in
   polynomial form.
*/
    //for (i = 0; i < kk; i++) data[i] = 0;


/* for example, say we transmit the following message (nothing special!) */
/*
    data[0] = 15;
    data[1] = 6;
    data[2] = 8;
    data[3] = 1;
    data[4] = 2;
    data[5] = 4;
    data[6] = 15;
    data[7] = 9;
    data[8] = 9;*/

/* encode data[] to produce parity in bb[].  Data input and parity output
   is in polynomial form
*/
    encode_rs();

/* put the transmitted codeword, made up of data plus parity, in recd[] */
    for (i = 0; i < nn - kk; i++) recd[i] = bb[i];
    for (i = 0; i < kk; i++) recd[i + nn - kk] = data[i];

/* if you want to test the program, corrupt some of the elements of recd[]
   here. This can also be done easily in a debugger. */
/* Again, lets say that a middle element is changed */
    //data[nn - nn / 2] = 3;

    for(i=2; i < kk-1; i++){
        data[i] = data[i+1];
    }

    for (i = 0; i < nn; i++)
        recd[i] = index_of[recd[i]];          /* put recd[i] into index form */

/* decode recv[] */
    decode_rs();         /* recd[] is returned in polynomial form */

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
        printf("%d         %X                   %X                    %X\n", i, orig_data[i], data[i], recd[nn-kk+i]);
    }

    //convert received polynomial back to struct
    telemetry_t rec = poly_to_struct();
    print_struct(&rec);

    return 0;
}
