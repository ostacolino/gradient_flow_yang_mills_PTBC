#ifndef TENS_PROD_C
#define TENS_PROD_C

#include "../include/macro.h"
#include "../include/endianness.h"
#include "../include/tens_prod.h"

#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// initialize to zero
void zero_TensProd(TensProd *A);

// initialize to one
void one_TensProd(TensProd *A);

// A=B
void equal_TensProd(TensProd *A, TensProd const *const B);

// A*=r real
void times_equal_real_TensProd(TensProd *A, double r);

// A*=r complex
void times_equal_complex_TensProd(TensProd *A, double complex r);

// A+=B
void plus_equal_TensProd(TensProd *A, TensProd const *const B);

// A=B*C
void times_TensProd(TensProd *A, TensProd const *const B, TensProd const *const C);

// A*=B
void times_equal_TensProd(TensProd *A, TensProd const *const B);

double retr_TensProd(TensProd const *const A);
double imtr_TensProd(TensProd const *const A);

void print_on_screen_TensProd(TensProd const *const A)
	{
	int i, j, k, l;

	for(i = 0; i < NCOLOR; i++)
		{
		for(j = 0; j < NCOLOR; j++)
			{
			for(k = 0; k < NCOLOR; k++)
				{
				for(l = 0; l < NCOLOR; l++) { printf("%.16lf %.16lf ", creal(A->comp[i][j][k][l]), cimag(A->comp[i][j][k][l])); }
				}
			}
		}
	printf("\n");
	}

void print_on_file_TensProd(FILE *fp, TensProd const *const A)
	{
	int i, j, k, l, err;

	for(i = 0; i < NCOLOR; i++)
		{
		for(j = 0; j < NCOLOR; j++)
			{
			for(k = 0; k < NCOLOR; k++)
				{
				for(l = 0; l < NCOLOR; l++)
					{
					err = fprintf(fp, "%.16lf %.16lf", creal(A->comp[i][j][k][l]), cimag(A->comp[i][j][k][l]));
					if(err != 2)
						{
						fprintf(stderr, "Problem in writing on file a TensProd (%s, %d)\n", __FILE__, __LINE__);
						exit(EXIT_FAILURE);
						}
					}
				}
			}
		}
	fprintf(fp, "\n");
	}

void print_on_binary_file_noswap_TensProd(FILE *fp, TensProd const *const A)
	{
	int i, j, k, l;
	size_t err;
	double re, im;

	for(i = 0; i < NCOLOR; i++)
		{
		for(j = 0; j < NCOLOR; j++)
			{
			for(k = 0; k < NCOLOR; k++)
				{
				for(l = 0; l < NCOLOR; l++)
					{
					re = creal(A->comp[i][j][k][l]);
					im = cimag(A->comp[i][j][k][l]);

					err = fwrite(&re, sizeof(double), 1, fp);
					if(err != 1)
						{
						fprintf(stderr, "Problem in binary writing on file a TensProd (%s, %d)\n", __FILE__, __LINE__);
						exit(EXIT_FAILURE);
						}
					err = fwrite(&im, sizeof(double), 1, fp);
					if(err != 1)
						{
						fprintf(stderr, "Problem in binary writing on file a TensProd (%s, %d)\n", __FILE__, __LINE__);
						exit(EXIT_FAILURE);
						}
					}
				}
			}
		}
	}

void print_on_binary_file_swap_TensProd(FILE *fp, TensProd const *const A)
	{
	int i, j, k, l;
	size_t err;
	double re, im;

	for(i = 0; i < NCOLOR; i++)
		{
		for(j = 0; j < NCOLOR; j++)
			{
			for(k = 0; k < NCOLOR; k++)
				{
				for(l = 0; l < NCOLOR; l++)
					{
					re = creal(A->comp[i][j][k][l]);
					im = cimag(A->comp[i][j][k][l]);

					SwapBytesDouble((void *)&re);
					SwapBytesDouble((void *)&im);

					err = fwrite(&re, sizeof(double), 1, fp);
					if(err != 1)
						{
						fprintf(stderr, "Problem in binary writing on file a TensProd (%s, %d)\n", __FILE__, __LINE__);
						exit(EXIT_FAILURE);
						}
					err = fwrite(&im, sizeof(double), 1, fp);
					if(err != 1)
						{
						fprintf(stderr, "Problem in binary writing on file a TensProd (%s, %d)\n", __FILE__, __LINE__);
						exit(EXIT_FAILURE);
						}
					}
				}
			}
		}
	}

void print_on_binary_file_bigen_TensProd(FILE *fp, TensProd const *const A)
	{
	if(endian() == 0) // little endian machine
		{
		print_on_binary_file_swap_TensProd(fp, A);
		}
	else
		{
		print_on_binary_file_noswap_TensProd(fp, A);
		}
	}

void read_from_file_TensProd(FILE *fp, TensProd *A)
	{
	int i, j, k, l, err;
	double re, im;

	for(i = 0; i < NCOLOR; i++)
		{
		for(j = 0; j < NCOLOR; j++)
			{
			for(k = 0; k < NCOLOR; k++)
				{
				for(l = 0; l < NCOLOR; l++)
					{
					err = fscanf(fp, "%lg %lg", &re, &im);
					if(err != 2)
						{
						fprintf(stderr, "Problems reading TensProd from file (%s, %d)\n", __FILE__, __LINE__);
						exit(EXIT_FAILURE);
						}
					A->comp[i][j][k][l] = re + I * im;
					}
				}
			}
		}
	}

void read_from_binary_file_noswap_TensProd(FILE *fp, TensProd *A)
	{
	int i, j, k, l;
	size_t err;
	double re, im;
	double aux[2];

	for(i = 0; i < NCOLOR; i++)
		{
		for(j = 0; j < NCOLOR; j++)
			{
			for(k = 0; k < NCOLOR; k++)
				{
				for(l = 0; l < NCOLOR; l++)
					{
					err = 0;

					err += fread(&re, sizeof(double), 1, fp);
					err += fread(&im, sizeof(double), 1, fp);
					if(err != 2)
						{
						fprintf(stderr, "Problems in binary reading TensProd file (%s, %d)\n", __FILE__, __LINE__);
						exit(EXIT_FAILURE);
						}

					aux[0] = re;
					aux[1] = im;

					memcpy((void *)&(A->comp[i][j][k][l]), (void *)aux, sizeof(aux));
					//equivalent to A->comp[i][j][k][l]=re+im*I;
					}
				}
			}
		}
	}

void read_from_binary_file_swap_TensProd(FILE *fp, TensProd *A)
	{
	int i, j, k, l;
	size_t err;
	double re, im;
	double aux[2];

	for(i = 0; i < NCOLOR; i++)
		{
		for(j = 0; j < NCOLOR; j++)
			{
			for(k = 0; k < NCOLOR; k++)
				{
				for(l = 0; l < NCOLOR; l++)
					{
					err = 0;

					err += fread(&re, sizeof(double), 1, fp);
					err += fread(&im, sizeof(double), 1, fp);
					if(err != 2)
						{
						fprintf(stderr, "Problems in binary reading TensProd file (%s, %d)\n", __FILE__, __LINE__);
						exit(EXIT_FAILURE);
						}

					SwapBytesDouble(&re);
					SwapBytesDouble(&im);
					aux[0] = re;
					aux[1] = im;

					memcpy((void *)&(A->comp[i][j][k][l]), (void *)aux, sizeof(aux));
					//equivalent to A->comp[i][j][k][l]=re+im*I;
					}
				}
			}
		}
	}

void read_from_binary_file_bigen_TensProd(FILE *fp, TensProd *A)
	{
	if(endian() == 0) // little endian machine
		{
		read_from_binary_file_swap_TensProd(fp, A);
		}
	else
		{
		read_from_binary_file_noswap_TensProd(fp, A);
		}
	}

#endif
