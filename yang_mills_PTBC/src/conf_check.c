#ifndef CONF_CHECK_C
#define CONF_CHECK_C

#include "../include/macro.h"

#ifdef HASH_MODE
#include <openssl/md5.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/function_pointers.h"

// get the spacetime dimension
void getspacetimedim(char *infile, int *dim)
	{
	int err;
	FILE *fp;

	fp = fopen(infile, "r");
	if(fp == NULL)
		{
		fprintf(stderr, "Error in opening the file %s (%s, %d)\n", infile, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
		}
	else
		{
		err = fscanf(fp, "%d", dim);
		if(err != 1)
			{
			fprintf(stderr, "Error in reading the file %s (%s, %d)\n", infile, __FILE__, __LINE__);
			exit(EXIT_FAILURE);
			}
		fclose(fp);
		}
	}

// read the size and the hash of the configuration
void getsizeandhash(char *infile, int *sides, char *hash)
	{
	FILE *fp;
	long update_index;
	int dim, err, i;

	fp = fopen(infile, "r");
	if(fp == NULL)
		{
		fprintf(stderr, "Error in opening the file %s (%s, %d)\n", infile, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
		}
	else
		{
		err = fscanf(fp, "%d", &dim);
		if(err != 1)
			{
			fprintf(stderr, "Error in reading the file %s (%s, %d)\n", infile, __FILE__, __LINE__);
			exit(EXIT_FAILURE);
			}

		for(i = 0; i < dim; i++)
			{
			err = fscanf(fp, "%d", &(sides[i]));
			if(err != 1)
				{
				fprintf(stderr, "Error in reading the file %s (%s, %d)\n", infile, __FILE__, __LINE__);
				exit(EXIT_FAILURE);
				}
			}

		err = fscanf(fp, "%ld %s", &update_index, hash);
		if(err != 2)
			{
			fprintf(stderr, "Error in reading the file %s (%s, %d)\n", infile, __FILE__, __LINE__);
			exit(EXIT_FAILURE);
			}
		fclose(fp);
		}
	}

// compute the hash
void computehash(char *infile, int dim, long volume, char *hash)
	{
	#ifdef HASH_MODE
	GAUGE_GROUP link;
	FILE *fp;
	long r;
	int j;

	MD5_CTX mdContext;
	unsigned char c[MD5_DIGEST_LENGTH];

	// open the configuration file in binary
	fp = fopen(infile, "rb");
	if(fp == NULL)
		{
		fprintf(stderr, "Error in opening the file %s (%s, %d)\n", infile, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
		}
	else
		{
		// read again the header:
		j = 0;
		while(j != '\n') { j = fgetc(fp); }

		// read the configuration & compute md5sum
		MD5_Init(&mdContext);
		for(r = 0; r < volume; r++)
			{
			for(j = 0; j < dim; j++)
				{
				read_from_binary_file_bigen(fp, &link);
				#if NCOLOR == 1
				MD5_Update(&mdContext, &(link.comp), sizeof(double complex));
				#elif NCOLOR == 2
				for(int k = 0; k < 4; k++) { MD5_Update(&mdContext, &(link.comp[k]), sizeof(double)); }
				#else
				for(int k = 0; k < NCOLOR * NCOLOR; k++) { MD5_Update(&mdContext, &(link.comp[k]), sizeof(double complex)); }
				#endif
				}
			}
		MD5_Final(c, &mdContext);
		for(r = 0; r < MD5_DIGEST_LENGTH; r++) { sprintf(&(hash[2 * r]), "%02x", c[r]); }

		fclose(fp);
		}
	#else
	// just to avoid compile time warnings
	(void)infile;
	(void)dim;
	(void)volume;
	*hash = '0';
	#endif
	}

// MAIN
int main(int argc, char **argv)
	{
	char infile[STD_STRING_LENGTH];
	int dim, *sides, error;

	#ifdef HASH_MODE
	long volumel;
	char md5sum_old[2 * MD5_DIGEST_LENGTH + 1];
	char md5sum_new[2 * MD5_DIGEST_LENGTH + 1];
	#else
	char md5sum_old[2 * STD_STRING_LENGTH + 1] = {0};
	#endif

	if(argc != 2)
		{
		printf("\nPackage %s version %s\n", PACKAGE_NAME, PACKAGE_VERSION);
		printf("Claudio Bonati %s\n", PACKAGE_BUGREPORT);
		printf("Usage: %s conf_file\n\n", argv[0]);

		printf("Compilation details:\n");
		printf("\tN_c (number of colors): %d\n", NCOLOR);
		printf("\n");
		printf("\tINT_ALIGN: %s\n", QUOTEME(INT_ALIGN));
		printf("\tDOUBLE_ALIGN: %s\n", QUOTEME(DOUBLE_ALIGN));

		return EXIT_SUCCESS;
		}
	else
		{
		if(strlen(argv[1]) >= STD_STRING_LENGTH) { fprintf(stderr, "File name too long. Increse STD_STRING_LENGTH in include/macro.h\n"); }
		else
			{
			strcpy(infile, argv[1]);
			}
		}

	// get spacetime dim
	getspacetimedim(infile, &dim);

	error = posix_memalign((void **)&sides, (size_t)INT_ALIGN, (size_t)dim * sizeof(int));
	if(error != 0)
		{
		fprintf(stderr, "Problems in allocating a vector! (%s, %d)\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
		}

	// get lattice size and initial hash
	getsizeandhash(infile, sides, md5sum_old);

	#ifdef HASH_MODE
	// total volume
	volumel = 1;
	for(int i = 0; i < dim; i++) { volumel *= sides[i]; }

	// compute the hash
	computehash(infile, dim, volumel, md5sum_new);

	// check md5sum computed and stored
	if(strncmp(md5sum_old, md5sum_new, 2 * MD5_DIGEST_LENGTH + 1) != 0)
		{
		fprintf(stderr, "The configuration %s is corrupted!\n", infile);
		return EXIT_FAILURE;
		}
	#endif

	free(sides);

	return EXIT_SUCCESS;
	}

#endif
