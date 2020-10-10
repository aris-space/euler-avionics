/*
 * RS.c
 *
 * Created on: 20.09.2020
 *     Author: Imre Kertesz
 *
 * This is an adaption of the implementation of the Reed-Solomon code created
 * by Simon Rockliff, 26th June 1991
 * The original implementation can be found at: https://github.com/ethz-asl/rtklibros/blob/master/src/todo/rs.c
 */

#include "Util/RS.h"

// array of irreducible polynomials
char *poly_array[] = {
        NULL,                   //  0   no code
        NULL,                   //  1   no code
        "111",                  //  2   1+x+x^2
        "1101",                 //  3   1+x+x^3
        "11001",                //  4   1+x+x^4
        "101001",               //  5   1+x^2+x^5
        "1100001",              //  6   1+x+x^6
        "10010001",             //  7   1+x^3+x^7
        "101110001",            //  8   1+x^2+x^3+x^4+x^8
        "1000100001",           //  9   1+x^4+x^9
        "10010000001",          // 10   1+x^3+x^10
        "101000000001",         // 11   1+x^2+x^11
        "1100101000001",        // 12   1+x+x^4+x^6+x^12
        "11011000000001",       // 13   1+x+x^3+x^4+x^13
        "110000100010001",      // 14   1+x+x^6+x^10+x^14
        "1100000000000001",     // 15   1+x+x^15
        "11010000000010001"     // 16   1+x+x^3+x^12+x^16
};

// sets the irreducible polynomial
void set_irr_poly(int16_t *pp){
    register int i;

    char *x = poly_array[MM];
    for (i=0; i < MM+1; i++) {
        pp[i] = x[i] - "0"[0];
    }
}

//generates the Galois field
void generate_gf(int16_t const *pp, int16_t *index_of, int16_t *alpha_to){
    register int i, mask;

    mask = 1;
    alpha_to[MM] = 0;
    for (i=0; i < MM; i++){
        alpha_to[i] = mask;
        index_of[alpha_to[i]] = i;
        if (pp[i] != 0)
            alpha_to[MM] ^= mask;
        mask <<= 1;
    }
    index_of[alpha_to[MM]] = MM;
    mask >>= 1;
    for (i = MM + 1; i < NN; i++) {
        if (alpha_to[i - 1] >= mask)
            alpha_to[i] = alpha_to[MM] ^ ((alpha_to[i - 1] ^ mask) << 1);
        else alpha_to[i] = alpha_to[i - 1] << 1;
        index_of[alpha_to[i]] = i;
    }
    index_of[0] = -1;
}

// creates the generator polynomial
void gen_poly(int16_t *gg, int16_t const *alpha_to, int16_t const *index_of)
/* Obtain the generator polynomial of the TT-error correcting, length
  NN=(2**mm -1) Reed Solomon code  from the product of (X+alpha**i), i=1..2*TT
*/
{

    gg[0] = 2;    /* primitive element alpha = 2  for GF(2**mm)  */
    gg[1] = 1;    /* g(x) = (X+alpha) initially */
    for (int i = 2; i <= NN - KK; i++) {
        gg[i] = 1;
        for (int j = i - 1; j > 0; j--)
            if (gg[j] != 0) gg[j] = gg[j - 1] ^ alpha_to[(index_of[gg[j]] + i) % NN];
            else gg[j] = gg[j - 1];
        gg[0] = alpha_to[(index_of[gg[0]] + i) % NN];     /* gg[0] can never be zero */
    }
    /* convert gg[] to index form for quicker encoding */
    for (int i = 0; i <= NN - KK; i++) gg[i] = index_of[gg[i]];
}

void encode_rs(int16_t *bb, int16_t const *index_of, int16_t const *alpha_to, int16_t const *gg, int16_t const *data)
/* take the string of symbols in data[i], i=0..(k-1) and encode systematically
   to produce 2*TT parity symbols in bb[0]..bb[2*TT-1]
   data[] is input and bb[] is output in polynomial form.
   Encoding is done by using a feedback shift register with appropriate
   connections specified by the elements of gg[], which was generated above.
   Codeword is   c(X) = data(X)*X**(NN-KK)+ b(X)          */
{
    int feedback;

    for (int i = 0; i < NN - KK; i++) bb[i] = 0;
    for (int i = KK - 1; i >= 0; i--) {
        feedback = index_of[data[i] ^ bb[NN - KK - 1]];
        if (feedback != -1) {
            for (int j = NN - KK - 1; j > 0; j--)
                if (gg[j] != -1)
                    bb[j] = bb[j - 1] ^ alpha_to[(gg[j] + feedback) % NN];
                else
                    bb[j] = bb[j - 1];
            bb[0] = alpha_to[(gg[0] + feedback) % NN];
        } else {
            for (int j = NN - KK - 1; j > 0; j--)
                bb[j] = bb[j - 1];
            bb[0] = 0;
        }
    }
}


void decode_rs(int16_t * recd, int16_t const *alpha_to, int16_t const *index_of)
/* assume we have received bits grouped into mm-bit symbols in recd[i],
   i=0..(NN-1),  and recd[i] is index form (ie as powers of alpha).
   We first compute the 2*TT syndromes by substituting alpha**i into rec(X) and
   evaluating, storing the syndromes in s[i], i=1..2tt (leave s[0] zero) .
   Then we use the Berlekamp iteration to find the error location polynomial
   elp[i].   If the degree of the elp is >TT, we cannot correct all the errors
   and hence just put out the information symbols uncorrected. If the degree of
   elp is <=TT, we substitute alpha**i , i=1..n into the elp to get the roots,
   hence the inverse roots, the error location numbers. If the number of errors
   located does not equal the degree of the elp, we have more than TT errors
   and cannot correct them.  Otherwise, we then solve for the error value at
   the error location and correct the error.  The procedure is that found in
   Lin and Costello. For the cases where the number of errors is known to be too
   large to correct, the information symbols as received are output (the
   advantage of systematic encoding is that hopefully some of the information
   symbols will be okay and that if we are in luck, the errors are in the
   parity part of the transmitted codeword).  Of course, these insoluble cases
   can be returned as error flags to the calling routine if desired.   */
{
    register int i, j, u, q;
    int elp[NN - KK + 2][NN - KK], d[NN - KK + 2], l[NN - KK + 2], u_lu[NN - KK + 2], s[NN - KK + 1];
    int count, syn_error = 0, root[TT], loc[TT], z[TT + 1], err[NN], reg[TT + 1];

/* first form the syndromes */
    for (i = 1; i <= NN - KK; i++) {
        s[i] = 0;
        for (j = 0; j < NN; j++)
            if (recd[j] != -1)
                s[i] ^= alpha_to[(recd[j] + i * j) % NN];      /* recd[j] in index form */
/* convert syndrome from polynomial form to index form  */
        if (s[i] != 0) syn_error = 1;        /* set flag if non-zero syndrome => error */
        s[i] = index_of[s[i]];
    }

    if (syn_error)       /* if errors, try and correct */
    {
/* compute the error location polynomial via the Berlekamp iterative algorithm,
   following the terminology of Lin and Costello :   d[u] is the 'mu'th
   discrepancy, where u='mu'+1 and 'mu' (the Greek letter!) is the step number
   ranging from -1 to 2*TT (see L&C),  l[u] is the
   degree of the elp at that step, and u_l[u] is the difference between the
   step number and the degree of the elp.
*/
/* initialise table entries */
        d[0] = 0;           /* index form */
        d[1] = s[1];        /* index form */
        elp[0][0] = 0;      /* index form */
        elp[1][0] = 1;      /* polynomial form */
        for (i = 1; i < NN - KK; i++) {
            elp[0][i] = -1;   /* index form */
            elp[1][i] = 0;   /* polynomial form */
        }
        l[0] = 0;
        l[1] = 0;
        u_lu[0] = -1;
        u_lu[1] = 0;
        u = 0;

        do {
            u++;
            if (d[u] == -1) {
                l[u + 1] = l[u];
                for (i = 0; i <= l[u]; i++) {
                    elp[u + 1][i] = elp[u][i];
                    elp[u][i] = index_of[elp[u][i]];
                }
            } else
/* search for words with greatest u_lu[q] for which d[q]!=0 */
            {
                q = u - 1;
                while ((d[q] == -1) && (q > 0)) q--;
/* have found first non-zero d[q]  */
                if (q > 0) {
                    j = q;
                    do {
                        j--;
                        if ((d[j] != -1) && (u_lu[q] < u_lu[j]))
                            q = j;
                    } while (j > 0);
                }

/* have now found q such that d[u]!=0 and u_lu[q] is maximum */
/* store degree of new elp polynomial */
                if (l[u] > l[q] + u - q) l[u + 1] = l[u];
                else l[u + 1] = l[q] + u - q;

/* form new elp(x) */
                for (i = 0; i < NN - KK; i++) elp[u + 1][i] = 0;
                for (i = 0; i <= l[q]; i++)
                    if (elp[q][i] != -1)
                        elp[u + 1][i + u - q] = alpha_to[(d[u] + NN - d[q] + elp[q][i]) % NN];
                for (i = 0; i <= l[u]; i++) {
                    elp[u + 1][i] ^= elp[u][i];
                    elp[u][i] = index_of[elp[u][i]];  /*convert old elp value to index*/
                }
            }
            u_lu[u + 1] = u - l[u + 1];

/* form (u+1)th discrepancy */
            if (u < NN - KK)    /* no discrepancy computed on last iteration */
            {
                if (s[u + 1] != -1)
                    d[u + 1] = alpha_to[s[u + 1]];
                else
                    d[u + 1] = 0;
                for (i = 1; i <= l[u + 1]; i++)
                    if ((s[u + 1 - i] != -1) && (elp[u + 1][i] != 0))
                        d[u + 1] ^= alpha_to[(s[u + 1 - i] + index_of[elp[u + 1][i]]) % NN];
                d[u + 1] = index_of[d[u + 1]];    /* put d[u+1] into index form */
            }
        } while ((u < NN - KK) && (l[u + 1] <= TT));

        u++;
        if (l[u] <= TT)         /* can correct error */
        {
/* put elp into index form */
            for (i = 0; i <= l[u]; i++) elp[u][i] = index_of[elp[u][i]];

/* find roots of the error location polynomial */
            for (i = 1; i <= l[u]; i++)
                reg[i] = elp[u][i];
            count = 0;
            for (i = 1; i <= NN; i++) {
                q = 1;
                for (j = 1; j <= l[u]; j++)
                    if (reg[j] != -1) {
                        reg[j] = (reg[j] + j) % NN;
                        q ^= alpha_to[reg[j]];
                    }
                if (!q)        /* store root and error location number indices */
                {
                    root[count] = i;
                    loc[count] = NN - i;
                    count++;
                }
            }
            if (count == l[u])    /* no. roots = degree of elp hence <= TT errors */
            {
/* form polynomial z(x) */
                for (i = 1; i <= l[u]; i++)        /* Z[0] = 1 always - do not need */
                {
                    if ((s[i] != -1) && (elp[u][i] != -1))
                        z[i] = alpha_to[s[i]] ^ alpha_to[elp[u][i]];
                    else if ((s[i] != -1) && (elp[u][i] == -1))
                        z[i] = alpha_to[s[i]];
                    else if ((s[i] == -1) && (elp[u][i] != -1))
                        z[i] = alpha_to[elp[u][i]];
                    else
                        z[i] = 0;
                    for (j = 1; j < i; j++)
                        if ((s[j] != -1) && (elp[u][i - j] != -1))
                            z[i] ^= alpha_to[(elp[u][i - j] + s[j]) % NN];
                    z[i] = index_of[z[i]];         /* put into index form */
                }

                /* evaluate errors at locations given by error location numbers loc[i] */
                for (i = 0; i < NN; i++) {
                    err[i] = 0;
                    if (recd[i] != -1)        /* convert recd[] to polynomial form */
                        recd[i] = alpha_to[recd[i]];
                    else recd[i] = 0;
                }
                for (i = 0; i < l[u]; i++)    /* compute numerator of error term first */
                {
                    err[loc[i]] = 1;       /* accounts for z[0] */
                    for (j = 1; j <= l[u]; j++)
                        if (z[j] != -1)
                            err[loc[i]] ^= alpha_to[(z[j] + j * root[i]) % NN];
                    if (err[loc[i]] != 0) {
                        err[loc[i]] = index_of[err[loc[i]]];
                        q = 0;     /* form denominator of error term */
                        for (j = 0; j < l[u]; j++)
                            if (j != i)
                                q += index_of[1 ^ alpha_to[(loc[j] + root[i]) % NN]];
                        q = q % NN;
                        err[loc[i]] = alpha_to[(err[loc[i]] - q + NN) % NN];
                        recd[loc[i]] ^= err[loc[i]];  /*recd[i] must be in polynomial form */
                    }
                }
            } else    /* no. roots != degree of elp => >TT errors and cannot solve */
                for (i = 0; i < NN; i++)        /* could return error flag if desired */
                    if (recd[i] != -1)        /* convert recd[] to polynomial form */
                        recd[i] = alpha_to[recd[i]];
                    else recd[i] = 0;     /* just output received codeword as is */
        } else         /* elp has degree has degree >TT hence cannot solve */
            for (i = 0; i < NN; i++)       /* could return error flag if desired */
                if (recd[i] != -1)        /* convert recd[] to polynomial form */
                    recd[i] = alpha_to[recd[i]];
                else recd[i] = 0;     /* just output received codeword as is */
    } else       /* no non-zero syndromes => no errors: output received codeword */
        for (i = 0; i < NN; i++)
            if (recd[i] != -1)        /* convert recd[] to polynomial form */
                recd[i] = alpha_to[recd[i]];
            else recd[i] = 0;
}


void struct_to_poly(telemetry_t t_data, int16_t *data){
    unsigned char *buffer = (unsigned char*)malloc(sizeof(t_data));
    memcpy(buffer, (const unsigned char *)&t_data, sizeof(t_data));
    uint8_t idx;
    uint8_t tmp = 0;
    for(int i=0;i<KK;i++){

        idx = i-(int)ceil((double)i/2);
        if(tmp==0){
            data[i] = buffer[idx] >> 4;
            tmp=1;
        }else{
            data[i] = buffer[idx] & 0x0F;
            tmp=0;
        }
    }
}

telemetry_t poly_to_struct(int16_t const *recd){
    uint8_t tmp2[KK];
    for(int i=0;i < KK; i++){
        tmp2[i] = recd[NN-KK+i];
    }
    unsigned char *rec_buffer = (unsigned char*)malloc(sizeof(telemetry_t));
    for(int i=0; i < KK2-1; i++){
        rec_buffer[i] = (tmp2[2*i] << 4) | tmp2[2*i+1];
    }
    telemetry_t rec;
    memcpy(&rec, rec_buffer, sizeof(rec));
    return rec;
}

void compress_data(int16_t const *recd, int16_t *recd_compact){
    for (int i=0;i < KK2-1;i++){
    	recd_compact[i] = (recd[2*i] << 4) | recd[2*i+1];
    }
    for (int i=0; i<NN-KK;i++){
            recd_compact[i+KK2-1] = recd[i+KK-1];
        }
}

int16_t * decompress_data(int16_t const *recd_compact){
    uint8_t idx, tmp = 0;
    static int16_t recd[NN];
    for (int i=0;i< NN-KK;i++){
        recd[i] = recd_compact[i];
    }
    for(int i=NN-KK;i<NN;i++){

        idx = (i-NN+KK)-(int)ceil((double)(i-NN+KK)/2)+NN-KK;
        if(tmp==0){
            recd[i] = recd_compact[idx] >> 4;
            tmp=1;
        }else{
            recd[i] = recd_compact[idx] & 0x0F;
            tmp=0;
        }

    }
    return recd;
}

void print_look_up_table(int16_t const * alpha_to, int16_t const *index_of){
    register int i;
    printf("\nLook-up tables for GF(2**%2d)\n", MM);
    printf("  i   alpha_to[i]  index_of[i]\n");
    for (i = 0; i <= NN; i++)
        printf("%3d      %3d          %3d\n", i, alpha_to[i], index_of[i]);
    printf("\n\n");
}
