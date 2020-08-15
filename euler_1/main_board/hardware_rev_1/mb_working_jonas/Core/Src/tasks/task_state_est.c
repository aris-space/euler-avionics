/*
 * task_state_est.c
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#include "tasks/task_state_est.h"

void Initialise_Kalman(float Ad[NUMBER_STATES][NUMBER_STATES],
		float Bd[NUMBER_STATES][NUMBER_INPUTS], float Gd[NUMBER_STATES][NUMBER_NOISE],
		float H[NUMBER_SENSOR][NUMBER_STATES], float Q[NUMBER_STATES][NUMBER_STATES],
		float R[NUMBER_SENSOR][NUMBER_SENSOR]);

void M_Calc_Size_A_vec(float A[NUMBER_STATES][NUMBER_STATES], float x[NUMBER_STATES], float y[NUMBER_STATES]);

float determinant(float a[NUMBER_SENSOR][NUMBER_SENSOR], float size);
void cofactor(float num[NUMBER_SENSOR][NUMBER_SENSOR], float inverse[NUMBER_SENSOR][NUMBER_SENSOR], float size);
void transpose(float inverse[NUMBER_SENSOR][NUMBER_SENSOR], float num[NUMBER_SENSOR][NUMBER_SENSOR], float fac[NUMBER_SENSOR][NUMBER_SENSOR], float size);
void pinv(float lambda, float Matrix[NUMBER_SENSOR][NUMBER_SENSOR], float Inverse[NUMBER_SENSOR][NUMBER_SENSOR]);

void vTaskStateEst(void *argument) {

	/* For periodic update */
	uint32_t tick_count, tick_update;

	/* Variables */

	/* Fixed Variables */
	float Ad[NUMBER_STATES][NUMBER_STATES] = { 0 };
	float Ad_T[NUMBER_STATES][NUMBER_STATES] = { 0 };
	float Bd[NUMBER_STATES][NUMBER_INPUTS] = { 0 };
	float Gd[NUMBER_STATES][NUMBER_NOISE] = { 0 };
	float Gd_T[NUMBER_NOISE][NUMBER_STATES] = { 0 };
	float H[NUMBER_SENSOR][NUMBER_STATES] = { 0 };
	float H_T[NUMBER_STATES][NUMBER_SENSOR] = { 0 };
	float Q[NUMBER_NOISE][NUMBER_NOISE] = { 0 };
	float R[NUMBER_SENSOR][NUMBER_SENSOR] = { 0 };
	float u[NUMBER_INPUTS] = { 0 };

	/* State Variables */
	float x_priori[NUMBER_STATES] = { 0 };
	float x_est[NUMBER_STATES] = { 0 };
	float P_priori[NUMBER_STATES][NUMBER_STATES] = { 0 };
	float P_est[NUMBER_STATES][NUMBER_STATES] = { 0 };
	float State_Inovation[NUMBER_SENSOR] = { 0 };
	float Cov_Inovation[NUMBER_SENSOR][NUMBER_SENSOR] = { 0 };
	float Cov_Inovation_Inv[NUMBER_SENSOR][NUMBER_SENSOR] = { 0 };
	float K[NUMBER_STATES][NUMBER_SENSOR] = { 0 };
	float measurements[NUMBER_SENSOR] = { 0 };

	/* Placeholder Variables */
	float Placeholder_priori[NUMBER_STATES][NUMBER_STATES] = { 0 };
	float Placeholder_est[NUMBER_SENSOR][NUMBER_SENSOR] = { 0 };
	float Placeholder_K[NUMBER_STATES][NUMBER_SENSOR] = { 0 };
	float Placeholder_P_est[NUMBER_STATES][NUMBER_STATES] = { 0 };

	/* Infinite loop */
	tick_count = osKernelGetTickCount();
	tick_update = osKernelGetTickFreq() / STATE_ESTIMATION_FREQUENCY;
	for (;;) {
		tick_count += tick_update;

		/* Prediction Step */
		/* Calculation of x_priori */
		memset(x_priori, 0, NUMBER_STATES*sizeof(x_priori[0]));
		for(int j = 0; j < NUMBER_STATES; j++){
			for(int i = 0; i < NUMBER_STATES; i++){
				x_priori[j] += Ad[j][i] * x_est[i];
			}
			for(int k = 0; k < NUMBER_INPUTS; k++){
				x_priori[j] += Bd[j][k] * u[k];
			}
		}

		/* Calculation of P_priori */
		/* Ad * P_est_prior * Ad_T */
		memset(Placeholder_priori, 0, NUMBER_STATES*NUMBER_STATES*sizeof(Placeholder_priori[0][0]));
		for(int j = 0; j < NUMBER_STATES; j++){
			for(int i = 0; i < NUMBER_STATES; i++){
				for(int k = 0; k < NUMBER_STATES; k++){
					Placeholder_priori[j][i] +=  Ad[j][k] * P_est[k][i];
				}
			}
		}
		memset(P_priori, 0, NUMBER_STATES*NUMBER_STATES*sizeof(P_priori[0][0]));
		for(int j = 0; j < NUMBER_STATES; j++){
			for(int i = 0; i < NUMBER_STATES; i++){
				for(int k = 0; k < NUMBER_STATES; k++){
					P_priori[j][i] +=  Placeholder_priori[j][k] * Ad_T[k][i];
				}
			}
		}

		/* Gd * Q * Gd_T */
		/* If Q is dimension 1, very easy -> do later */

		/* Update Step */
		/* state_inov = measurement - H * x_priori */
		/* H*P_priori */
		memset(State_Inovation, 0, NUMBER_SENSOR*sizeof(State_Inovation[0]));
		memset(Placeholder_est, 0, NUMBER_SENSOR*NUMBER_STATES*sizeof(Placeholder_est[0][0]));
		for(int j = 0; j < NUMBER_SENSOR; j++){
			for(int i = 0; i < NUMBER_STATES; i++){
				State_Inovation[j] += measurements[j] - H[j][i]*x_priori[i];
				for(int k = 0; k < NUMBER_STATES; k++){
					Placeholder_est[j][i] += H[j][k] * P_priori[k][i];
				}
			}
		}

		/* CHECK AGAIN STARTING HERE */

		/* S = H*P*H_T + R */
		memset(Cov_Inovation, 0, NUMBER_SENSOR*NUMBER_SENSOR*sizeof(Cov_Inovation[0][0]));
		for(int j = 0; j < NUMBER_SENSOR; j++){
			for(int i = 0; i < NUMBER_SENSOR; i++){
				for(int k = 0; k < NUMBER_STATES; k++){
					Cov_Inovation[j][i] += Placeholder_est[j][k] * H_T[k][i];
				}
				Cov_Inovation[j][i] += R[j][i];
			}
		}

		/* Calculate Pseudoinverse of Cov_Inovation */
		pinv(LAMBDA, Cov_Inovation, Cov_Inovation_Inv);

		/* K  = P_priori * H_T * Cov_Inovation_Inv */
		memset(Placeholder_K, 0, NUMBER_STATES*NUMBER_SENSOR*sizeof(Placeholder_K[0][0]));
		for(int j = 0; j < NUMBER_STATES; j++){
			for(int i = 0; i < NUMBER_SENSOR; i++){
				for(int k = 0; k < NUMBER_STATES; k++){
					Placeholder_K[j][i] += P_priori[j][k] * H_T[k][i];
				}
			}
		}
		memset(K, 0, NUMBER_STATES*NUMBER_SENSOR*sizeof(K[0][0]));
		for(int j = 0; j < NUMBER_STATES; j++){
			for(int i = 0; i < NUMBER_SENSOR; i++){
				for(int k = 0; k < NUMBER_SENSOR; k++){
					K[j][i] += Placeholder_K[j][k] * Cov_Inovation_Inv[k][i];
				}
			}
		}

		/* x_est = x_priori + K*measurements */
		memset(x_est, 0, NUMBER_STATES*sizeof(x_est[0]));
		for(int j = 0; j < NUMBER_STATES; j++){
			for(int i = 0; i < NUMBER_SENSOR; i++){
				x_est[j] += x_priori[j] + K[j][i]*State_Inovation[i];
			}
		}


		/* P_est = (eye(NUMBER_STATES) - K*H)*P_priori */
		memset(Placeholder_P_est, 0, NUMBER_STATES*NUMBER_STATES*sizeof(Placeholder_P_est[0][0]));
		for(int j = 0; j < NUMBER_STATES; j++){
			for(int i = 0; i < NUMBER_STATES; i++){
				for(int k = 0; k < NUMBER_SENSOR; k++){
					Placeholder_P_est[j][i] -= K[j][k] * H[k][i];
				}
				if(j == i){
					Placeholder_P_est[j][i] += 1;
				}
			}
		}
		for(int j = 0; j < NUMBER_STATES; j++){
			for(int i = 0; i < NUMBER_STATES; i++){
				for(int k = 0; k < NUMBER_STATES; k++){
					P_est[j][i] += Placeholder_P_est[j][k] * P_priori[k][i];
				}
			}
		}

		/* KALMAN UPDATE FINISHED */
		/* OUTPUT IS x_est */




		osDelayUntil(tick_count);
	}
}


void Initialise_Kalman(float Ad[NUMBER_STATES][NUMBER_STATES],
		float Bd[NUMBER_STATES][NUMBER_INPUTS], float Gd[NUMBER_STATES][NUMBER_NOISE],
		float H[NUMBER_SENSOR][NUMBER_STATES], float Q[NUMBER_STATES][NUMBER_STATES],
		float R[NUMBER_SENSOR][NUMBER_SENSOR]){

	float A_init[NUMBER_STATES][NUMBER_STATES] = {{0, 1, 0}, {0, 0, 1}, {0, 0, 0}};
	float B_init[NUMBER_STATES][NUMBER_INPUTS] = {{0}, {1}, {0}};
	float G_init[NUMBER_STATES][NUMBER_NOISE] = {{0}, {1}, {0}};
	float H_init[NUMBER_SENSOR][NUMBER_STATES] = {{1, 0, 0},{1, 0, 0}, {1, 0, 0}};
	float Q_init[NUMBER_STATES][NUMBER_STATES] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
	float R_init[NUMBER_SENSOR][NUMBER_SENSOR] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};


	H = H_init;
	Q = Q_init;
	R = R_init;
	Ad = A_init;
	Bd = B_init;
	Gd = G_init;


}

void pinv(float lambda, float Matrix[NUMBER_SENSOR][NUMBER_SENSOR], float Inverse[NUMBER_SENSOR][NUMBER_SENSOR]){

	/* More Penrose Pseudoinverse */
	/* pinv = A_T*(A*A_T*lambda^2*eye(size(A)))^-1 */
	float Matrix_T[NUMBER_SENSOR][NUMBER_SENSOR] = { 0 };
	float Matrix_to_inv[NUMBER_SENSOR][NUMBER_SENSOR] = { 0 };
	float First_Inverse[NUMBER_SENSOR][NUMBER_SENSOR] = { 0 };


	for(int j = 0; j < NUMBER_SENSOR; j++){
		for(int i = 0; i < NUMBER_SENSOR; i++){
			for(int k = 0; k < NUMBER_SENSOR; k++){
				Matrix_to_inv[j][i] +=  Matrix[j][k] * Matrix_T[k][i];
				if(i == j){
					Matrix_to_inv[j][i] += lambda * lambda;
				}
			}
		}
	}

	cofactor(Matrix_to_inv, First_Inverse, NUMBER_SENSOR);
	for(int j = 0; j < NUMBER_SENSOR; j++){
		for(int i = 0; i < NUMBER_SENSOR; i++){
			for(int k = 0; k < NUMBER_SENSOR; k++){
				Inverse[j][i] +=  Matrix_T[j][k] * First_Inverse[k][i];
			}
		}
	}


}

/*For calculating Determinant of the Matrix */
float determinant(float a[NUMBER_SENSOR][NUMBER_SENSOR], float size)
{
	float s = 1, det = 0, b[NUMBER_SENSOR][NUMBER_SENSOR];
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
			det = det + s * (a[0][c] * determinant(b, size - 1));
			s = -1 * s;
		}
	}

	return (det);
}

void cofactor(float num[NUMBER_SENSOR][NUMBER_SENSOR], float inverse[NUMBER_SENSOR][NUMBER_SENSOR], float size)
{
	float b[NUMBER_SENSOR][NUMBER_SENSOR], fac[NUMBER_SENSOR][NUMBER_SENSOR];
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
			fac[q][p] = pow(-1, q + p) * determinant(b, size - 1);
		}
	}
	transpose(inverse, num, fac, size);
}
/*Finding transpose of matrix*/
void transpose(float inverse[NUMBER_SENSOR][NUMBER_SENSOR], float num[NUMBER_SENSOR][NUMBER_SENSOR], float fac[NUMBER_SENSOR][NUMBER_SENSOR], float size)
{
	int i, j;
	float b[NUMBER_SENSOR][NUMBER_SENSOR], d;

	for (i = 0;i < size; i++)
	{
		for (j = 0;j < size; j++)
		{
			b[i][j] = fac[j][i];
		}
	}
	d = determinant(num, size);
	for (i = 0;i < size; i++)
	{
		for (j = 0;j < size; j++)
		{
			inverse[i][j] = b[i][j] / d;
		}
	}
}
