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

void transpose(int m, int n, float A[m][n], float A_T[n][m]) {
	/* Get Transpose */
	for(int i = 0; i < m; i++){
		for(int j = 0; j < n; j++){
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

void matadd(int m, int n, float A[m][n], float B[m][n], float C[m][n]) {
	for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            C[i][j] =  A[i][j] + B[i][j];
        }
    }
}

void matsub(int m, int n, float A[m][n], float B[m][n], float C[m][n]) {
	for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            C[i][j] =  A[i][j] - B[i][j];
        }
    }
}

void matmul(int m, int n, int o, float A[m][n], float B[n][o], float C[m][o], bool reset) {
	if (reset) {
		memset(C, 0, m * o * sizeof(C[0][0]));
	}
	for(int i = 0; i < m; i++){
        for(int j = 0; j < o; j++){
            for(int k = 0; k < n; k++){
                C[i][j] +=  A[i][k] * B[k][j];
            }
        }
    }
}

void matvecprod(int m, int n, float A[m][n], float b[n], float c[m], bool reset) {
	if (reset) {
		memset(c, 0, m * sizeof(c[0]));
	}
	for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            c[i] += A[i][j] * b[j];
        }
    }
}

void scalarmatprod(int m, int n, float scalar, float A[m][n], float B[m][n]) {
	for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            B[i][j] = scalar * A[i][j];
        }
    }
}

float vecsum(int n, float a[n]) {
	float sum = 0;
	for(int i = 0; i < n; i++){
        sum += a[i];
    }
	return sum;
}

/* Function to get cofactor of A[p][q] in temp[][]. n is current dimension of A[][] */
/* https://www.geeksforgeeks.org/adjoint-inverse-matrix/ */
void cofactor(int dim, float A[dim][dim], float temp[dim][dim], int p, int q, int n)
{
    int i = 0, j = 0;

    // Looping for each element of the matrix
    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            //  Copying into temporary matrix only those element
            //  which are not in given row and column
            if (row != p && col != q)
            {
                temp[i][j++] = A[row][col];

                // Row is filled, so increase row index and
                // reset col index
                if (j == n - 1)
                {
                    j = 0;
                    i++;
                }
            }
        }
    }
}

/* Recursive function for finding determinant of matrix. n is current dimension of A[][]. */
/* https://www.geeksforgeeks.org/adjoint-inverse-matrix/ */
float determinant(int dim, float A[dim][dim], int n)
{
    float D = 0; // Initialize result

    //  Base case : if matrix contains single element
    if (n == 1)
        return A[0][0];

    float temp[dim][dim]; // To store cofactors

    int sign = 1;  // To store sign multiplier

     // Iterate for each element of first row
    for (int f = 0; f < n; f++)
    {
        // Getting Cofactor of A[0][f]
        cofactor(dim, A, temp, 0, f, n);
        D += sign * A[0][f] * determinant(dim, temp, n - 1);

        // terms are to be added with alternate sign
        sign = -sign;
    }

    return D;
}

/* Function to get adjoint of A[dim][dim] in adj[dim][dim]. */
/* https://www.geeksforgeeks.org/adjoint-inverse-matrix/ */
void adjoint(int dim, float A[dim][dim], float adj[dim][dim])
{
    if (dim == 1)
    {
        adj[0][0] = 1;
        return;
    }

    // temp is used to store cofactors of A[][]
    int sign = 1;
	float temp[dim][dim];

    for (int i=0; i<dim; i++)
    {
        for (int j=0; j<dim; j++)
        {
            // Get cofactor of A[i][j]
            cofactor(dim, A, temp, i, j, dim);

            // sign of adj[j][i] positive if sum of row
            // and column indexes is even.
            sign = ((i+j)%2==0)? 1: -1;

            // Interchanging rows and columns to get the
            // transpose of the cofactor matrix
            adj[j][i] = (sign)*(determinant(dim, temp, dim-1));
        }
    }
}

/* Function to calculate and store inverse, returns false if matrix is singular */
/* https://www.geeksforgeeks.org/adjoint-inverse-matrix/ */
bool inverse(int dim, float A[dim][dim], float A_inv[dim][dim], float lambda)
{
	/* add damping factor to avoid singularities. */
	/* if no damping is required set lambda to 0.0 */
	float A_dash[dim][dim];
	float lambda_diag[dim][dim];
	eye(dim, lambda_diag);
	scalarmatprod(dim, dim, lambda, lambda_diag, lambda_diag);
	matadd(dim, dim, A, lambda_diag, A_dash);

    // Find determinant of A[][]
    float det = determinant(dim, A_dash, dim);

    if (det == 0)
    {
        printf("Singular matrix, can't find its inverse\n");
        return false;
    }

    // Find adjoint
    float adj[dim][dim];
    adjoint(dim, A_dash, adj);

    // Find Inverse using formula "inverse(A) = adj(A)/det(A)"
    for (int i=0; i<dim; i++) {
        for (int j=0; j<dim; j++) {
            A_inv[i][j] = adj[i][j] / ((float) det);
		}
	}

    return true;
}

/* Damped Moore-Penrose pseudo-inverse - ETHZ Robot Dynamics Lecture notes */
bool pseudo_inverse(int m, int n, float A[m][n], float A_inv[n][m], float lambda) {
	float A_T[n][m];
	transpose(m, n, A, A_T);

	bool inversible;
    if (m >= n) {
		/* we need to calculate left pseudo-inverse */
		float A_int[n][n];
		matmul(n, m, n, A_T, A, A_int, true);

		float A_int_inv[n][n];
		inversible = inverse(n, A_int, A_int_inv, lambda);
		if (inversible == true){
			matmul(n, n, m, A_int_inv, A_T, A_inv, true);
			return true;
		}
	} else {
		/* we need to calculate right pseudo-inverse */
		float A_int[m][m];
		matmul(m, n, m, A, A_T, A_int, true);

		float A_int_inv[m][m];
		inversible = inverse(m, A_int, A_int_inv, lambda);
		if (inversible == true){
			matmul(n, m, m, A_T, A_int_inv, A_inv, true);
			return true;
		}
	}
	return false;
}

//----------------------------------------------------
// SOURCE: https://github.com/natedomin/polyfit/blob/master/polyfit.c
//
// METHOD:  polyfit
//
// INPUTS:  dependentValues[0..(countOfElements-1)]
//          independentValues[0...(countOfElements-1)]
//          countOfElements
//          order - Order of the polynomial fitting
//
// OUTPUTS: coefficients[0..order] - indexed by term
//               (the (coef*x^3) is coefficients[3])
//
//----------------------------------------------------
int polyfit(const double* const dependentValues,
            const double* const independentValues,
            unsigned int        countOfElements,
            unsigned int        order,
            double*             coefficients)
{
    // Declarations...
    // ----------------------------------
    enum {maxOrder = 5};

    double B[maxOrder+1] = {0.0f};
    double P[((maxOrder+1) * 2)+1] = {0.0f};
    double A[(maxOrder + 1)*2*(maxOrder + 1)] = {0.0f};

    double x, y, powx;

    unsigned int ii, jj, kk;

    // Verify initial conditions....
    // ----------------------------------

    // This method requires that the countOfElements >
    // (order+1)
    if (countOfElements <= order)
        return -1;

    // This method has imposed an arbitrary bound of
    // order <= maxOrder.  Increase maxOrder if necessary.
    if (order > maxOrder)
        return -1;

    // Begin Code...
    // ----------------------------------

    // Identify the column vector
    for (ii = 0; ii < countOfElements; ii++)
    {
        x    = dependentValues[ii];
        y    = independentValues[ii];
        powx = 1;

        for (jj = 0; jj < (order + 1); jj++)
        {
            B[jj] = B[jj] + (y * powx);
            powx  = powx * x;
        }
    }

    // Initialize the PowX array
    P[0] = countOfElements;

    // Compute the sum of the Powers of X
    for (ii = 0; ii < countOfElements; ii++)
    {
        x    = dependentValues[ii];
        powx = dependentValues[ii];

        for (jj = 1; jj < ((2 * (order + 1)) + 1); jj++)
        {
            P[jj] = P[jj] + powx;
            powx  = powx * x;
        }
    }

    // Initialize the reduction matrix
    //
    for (ii = 0; ii < (order + 1); ii++)
    {
        for (jj = 0; jj < (order + 1); jj++)
        {
            A[(ii * (2 * (order + 1))) + jj] = P[ii+jj];
        }

        A[(ii*(2 * (order + 1))) + (ii + (order + 1))] = 1;
    }

    // Move the Identity matrix portion of the redux matrix
    // to the left side (find the inverse of the left side
    // of the redux matrix
    for (ii = 0; ii < (order + 1); ii++)
    {
        x = A[(ii * (2 * (order + 1))) + ii];
        if (x != 0)
        {
            for (kk = 0; kk < (2 * (order + 1)); kk++)
            {
                A[(ii * (2 * (order + 1))) + kk] =
                    A[(ii * (2 * (order + 1))) + kk] / x;
            }

            for (jj = 0; jj < (order + 1); jj++)
            {
                if ((jj - ii) != 0)
                {
                    y = A[(jj * (2 * (order + 1))) + ii];
                    for (kk = 0; kk < (2 * (order + 1)); kk++)
                    {
                        A[(jj * (2 * (order + 1))) + kk] =
                            A[(jj * (2 * (order + 1))) + kk] -
                            y * A[(ii * (2 * (order + 1))) + kk];
                    }
                }
            }
        }
        else
        {
            // Cannot work with singular matrices
            return -1;
        }
    }

    // Calculate and Identify the coefficients
    for (ii = 0; ii < (order + 1); ii++)
    {
        for (jj = 0; jj < (order + 1); jj++)
        {
            x = 0;
            for (kk = 0; kk < (order + 1); kk++)
            {
                x = x + (A[(ii * (2 * (order + 1))) + (kk + (order + 1))] *
                    B[kk]);
            }
            coefficients[ii] = x;
        }
    }

    return 0;
}
