#include <string.h>
#include <stdbool.h>
#include <math.h>

#ifndef MATH_UTILS_H_
#define MATH_UTILS_H_

/* Calculates the transpose of a Matrix */
void eye(int dim, float A[dim][dim]);

/* Calculates the transpose of a Matrix */
void transpose(int n, int m, float A[n][m], float A_T[m][n]);

/* Calculates the addition of two vectors */
void vecadd(int n, float a[n], float b[n], float c[n]);

/* Calculates the substraction of two vectors */
void vecsub(int n, float a[n], float b[n], float c[n]);

/* Calculates the addition of two matrices */
void matadd(int n, int m, float A[n][m], float B[n][m], float C[n][m]);

/* Calculates the substraction of two matrices */
void matsub(int n, int m, float A[n][m], float B[n][m], float C[n][m]);

/* Calculates the matrix multiplication of two matrices */
void matmul(int n, int m, int o, float A[n][m], float B[m][o], float C[n][o], bool reset);

/* Calculates the product of a matrix and a vector */
void matvecprod(int n, int m, float A[n][m], float b[m], float c[n], bool reset);

/* Calculates the determinant of a Matrix for inverse */
float determinant(int dim, float a[dim][dim], float size);

/* Calculated cofactor for Matrix inverse */
void cofactor(int dim, float num[dim][dim], float inverse[dim][dim], float size);

/* Transpose for matrix inverse */
void pinv_transpose(int dim, float inverse[dim][dim], float num[dim][dim], float fac[dim][dim], float size);

/* Moore Penrose Pseudoinverse */
void pinv(int dim, float lambda, float Matrix[dim][dim], float Inverse[dim][dim]);

#endif
