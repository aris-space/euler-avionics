#ifndef MATH_UTILS_H_
#define MATH_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

#define max(x, y) ((x) >= (y)) ? (x) : (y)
#define min(x, y) ((x) <= (y)) ? (x) : (y)

/* Calculates the transpose of a Matrix */
void eye(int32_t dim, float A[dim][dim]);

/* Calculates the transpose of a Matrix */
void transpose(int32_t m, int32_t n, float A[m][n], float A_T[n][m]);

/* Calculates the addition of two vectors */
void vecadd(int32_t n, float a[n], float b[n], float c[n]);

/* Calculates the substraction of two vectors */
void vecsub(int32_t n, float a[n], float b[n], float c[n]);

/* Calculates the addition of two matrices */
void matadd(int32_t m, int32_t n, float A[m][n], float B[m][n], float C[m][n]);

/* Calculates the substraction of two matrices */
void matsub(int32_t m, int32_t n, float A[m][n], float B[m][n], float C[m][n]);

/* Calculates the matrix multiplication of two matrices */
void matmul(int32_t n, int32_t m, int32_t o, float A[n][m], float B[m][o],
            float C[n][o], bool reset);

/* Calculates the product of a matrix and a vector */
void matvecprod(int32_t m, int32_t n, float A[m][n], float b[n], float c[m],
                bool reset);

/* Calculates the product of a scalar with a matrix */
void scalarmatprod(int32_t m, int32_t n, float scalar, float A[m][n],
                   float B[m][n]);

/* Calculates sum of all element in the vector */
float vecsum(int32_t n, float a[n]);

/* Function to get cofactor of A[p][q] in temp[][]. n is current dimension of
 * A[][] */
void cofactor(int32_t dim, float A[dim][dim], float temp[dim][dim], int32_t p,
              int32_t q, int32_t n);

/* Recursive function for finding determinant of matrix. n is current dimension
 * of A[][]. */
float determinant(int32_t dim, float A[dim][dim], int32_t n);

/* Function to get adjoint32_t of A[dim][dim] in adj[dim][dim]. */
void adjoint(int32_t dim, float A[dim][dim], float adj[dim][dim]);

/* Function to calculate and store inverse, returns false if matrix is singular
 */
bool inverse(int32_t dim, float A[dim][dim], float inverse[dim][dim],
             float lambda);

/* Damped Moore-Penrose pseudo-inverse - ETHZ Robot Dynamics Lecture notes */
bool pseudo_inverse(int32_t m, int32_t n, float A[m][n], float inverse[n][m],
                    float lambda);

/* the inverse of a matrix which has only diagonal elements */
void diag_inverse(int32_t n, float A[n][n], float inverse[n][n], float lambda);

/* computes the inverse of the lower triangular matrix L */
/* http://www.mymathlib.com/matrices/linearsystems/triangular.html */
int32_t lower_triangular_inverse(int32_t n, float *L);

/* computes the cholesky decomposition */
/* https://rosettacode.org/wiki/Cholesky_decomposition#C */
void cholesky(int32_t n, float A[n][n], float L[n][n]);

/* computes the inverse of a Hermitian, positive-definite matrix of dimension n
 * x n using cholesky decomposition*/
/* Krishnamoorthy, Aravindh, and Deepak Menon. "Matrix inversion using Cholesky
 * decomposition." */
/* 2013 signal processing: Algorithms, architectures, arrangements, and
 * applications (SPA). IEEE, 2013. */
/* the inverse has a big O complexity of n^3 */
void cholesky_inverse(int32_t n, float A[n][n], float inverse[n][n],
                      float lambda);

/* Polyfit https://github.com/natedomin/polyfit/blob/master/polyfit.c */
int32_t polyfit(const float *const dependentValues,
                const float *const independentValues, uint32_t countOfElements,
                uint32_t order, double *coefficients);

#endif
