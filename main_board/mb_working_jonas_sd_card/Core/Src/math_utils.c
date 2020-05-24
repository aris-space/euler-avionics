#include "math_utils.h"


void eye(int dim, float A[dim][dim]) {
	for(int i = 0; i < dim; i++){
        for(int j = 0; j < dim; j++){
            if (i == j){
				A[i][j] = 1;
			} else {
				A[i][j] = 0;
			}
        }
    }
}

void transpose(int n, int m, float A[n][m], float A_T[m][n]) {
	/* Get Transpose */
	for(int i = 0; i < n; i++){
		for(int j = 0; j < m; j++){
			A_T[j][i] = A[i][j];
		}
	}
}

void vecadd(int n, float a[n], float b[n], float c[n]) {
	for(int i = 0; i < n; i++){
		c[i] = a[i] + b[i];
	}
}

void vecsub(int n, float a[n], float b[n], float c[n]) {
	for(int i = 0; i < n; i++){
		c[i] = a[i] - b[i];
	}
}

void matadd(int n, int m, float A[n][m], float B[n][m], float C[n][m]) {
	for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            C[i][j] =  A[i][j] + B[i][j];
        }
    }
}

void matsub(int n, int m, float A[n][m], float B[n][m], float C[n][m]) {
	for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            C[i][j] =  A[i][j] - B[i][j];
        }
    }
}

void matmul(int n, int m, int o, float A[n][m], float B[m][o], float C[n][o], bool reset) {
	if (reset) {
		memset(C, 0, n*o*sizeof(C[0][0]));
	}
	for(int i = 0; i < n; i++){
        for(int j = 0; j < o; j++){
            for(int k = 0; k < m; k++){
                C[i][j] +=  A[i][k] * B[k][j];
            }
        }
    }
}

void matvecprod(int n, int m, float A[n][m], float b[m], float c[n], bool reset) {
	if (reset) {
		memset(c, 0, n*sizeof(c[0]));
	}
	for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            c[i] += A[i][j] * b[j];
        }
    }
}

void pinv(int dim, float lambda, float Matrix[dim][dim], float Inverse[dim][dim]){

	/* Moore Penrose Pseudoinverse */
	/* pinv = A_T*(A*A_T*lambda^2*eye(size(A)))^-1 */
	float Matrix_T[dim][dim];
	memset(Matrix_T, 0, dim*dim);
	float Matrix_to_inv[dim][dim];
	memset(Matrix_to_inv, 0, dim*dim);
	float First_Inverse[dim][dim];
	memset(First_Inverse, 0, dim*dim);

	transpose(dim, dim, Matrix, Matrix_T);

	/* Calculate the Matrix to be inversed A*A_T*lambda^2*eye(size(A))*/
	for(int j = 0; j < dim; j++){
		for(int i = 0; i < dim; i++){
			for(int k = 0; k < dim; k++){
				Matrix_to_inv[j][i] +=  Matrix[j][k] * Matrix_T[k][i];
				if(i == j){
					Matrix_to_inv[j][i] += lambda * lambda;
				}
			}
		}
	}

	/* Calculate inverse */
	cofactor(dim, Matrix_to_inv, First_Inverse, dim);
	for(int j = 0; j < dim; j++){
		for(int i = 0; i < dim; i++){
			for(int k = 0; k < dim; k++){
				Inverse[j][i] +=  Matrix_T[j][k] * First_Inverse[k][i];
			}
		}
	}
}

/*For calculating Determinant of the Matrix */
float determinant(int dim, float a[dim][dim], float size)
{
	float s = 1, det = 0, b[dim][dim];
	int i, j, m, n, c;
	if (size == 1)
	{
		return (a[0][0]);
	}
	else
	{
		det = 0;
		for (c = 0; c < size; c++)
		{
			m = 0;
			n = 0;
			for (i = 0;i < size; i++)
			{
				for (j = 0 ;j < size; j++)
				{
					b[i][j] = 0;
					if (i != 0 && j != c)
					{
						b[m][n] = a[i][j];
						if (n < (size - 2))
							n++;
						else
						{
							n = 0;
							m++;
						}
					}
				}
			}
			det = det + s * (a[0][c] * determinant(dim, b, size - 1));
			s = -1 * s;
		}
	}

	return (det);
}

void cofactor(int dim, float num[dim][dim], float inverse[dim][dim], float size)
{
	float b[dim][dim], fac[dim][dim];
	int p, q, m, n, i, j;
	for (q = 0;q < size; q++)
	{
		for (p = 0;p < size; p++)
		{
			m = 0;
			n = 0;
			for (i = 0;i < size; i++)
			{
				for (j = 0;j < size; j++)
				{
					if (i != q && j != p)
					{
						b[m][n] = num[i][j];
						if (n < (size - 2))
							n++;
						else
						{
							n = 0;
							m++;
						}
					}
				}
			}
			fac[q][p] = pow(-1, q + p) * determinant(dim, b, size - 1);
		}
	}
	pinv_transpose(dim, inverse, num, fac, size);
}
/*Finding transpose of matrix*/
void pinv_transpose(int dim, float inverse[dim][dim], float num[dim][dim], float fac[dim][dim], float size)
{
	int i, j;
	float b[dim][dim], d;

	for (i = 0;i < size; i++)
	{
		for (j = 0;j < size; j++)
		{
			b[i][j] = fac[j][i];
		}
	}
	d = determinant(dim, num, size);

	for (i = 0;i < size; i++)
	{
		for (j = 0;j < size; j++)
		{
			inverse[i][j] = b[i][j] / d;
		}
	}
}
