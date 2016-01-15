#include <stdio.h>
#include <strings.h>
#include <nifti1.h>
#include <fslio.h>
#include <nifti1_io.h>
#include <math.h>
#include <omp.h>

/* A function to check for bugs in the result matrix */
void checkMat(double **matrix, int goodValues) {
        int nans = 0;
	int maxs = 0;
	int zeros = 0;
	long total;
	total = goodValues*goodValues;
	double pnan, pmax, pzeros;
        for ( int i = 0 ; i < goodValues ; i++ ) {
                for ( int j = 0 ; j < goodValues ; j++ ) {
                        if ( i == j && matrix[i][j] < 0.99 ) {
				printf("checkMat: diagonal at index %i is not 1!!! \n", i);
				matrix[i][j] = 1;
                        } else {
				if ( matrix[i][j] != matrix[j][i] ) {
					printf("checkMat: matrix is not symmetric! did not pass test at index (%i %i) \n", i, j);
				}
			}
                }
        }
}
/* main function */
int main ( int argc , char * argv[] ) {
	int **del;
	long c1, c2, c3;
	int err, atDelay, atDelay2;
	double corr, corr2, corr_nd, corr2_nd;
	/* parsing input params */
	char *inputFile, *maskFile, *fout1;;
	inputFile = argv[1]; 
	fout1 = argv[2];

	// Because this program is written in such a hurry, HARDCODING!
	int goodValues = 28146;

	fprintf(stderr, "Allocating some memory... ");	
	double *inputArray;
	inputArray = (double*)calloc(goodValues*goodValues, sizeof(double));
	double **out;
        out = (double**)calloc(goodValues,sizeof(double*));
	for(int i = 0; i < goodValues; i++ ) out[i] = (double*)calloc(goodValues,sizeof(double));
	fprintf(stderr, "...DONE!\n");	

	fprintf(stderr, "Reading data ...  ");	
	FILE * file;
	file = fopen( inputFile , "r" );
	double d;
	for ( int i = 0 ; i < goodValues*goodValues ; i++ ) {
        	fscanf( file, "%lf", &d );
		inputArray[i] = d;
	}
	fprintf(stderr, "...DONE!\n");	
	
	fprintf(stderr, "**performing 2d matrix transfer ... \n");
	#pragma omp parallel for
	for ( int i = 0 ; i < goodValues ; i++ ) {
		for ( int j = 0 ; j < goodValues ; j++ ) {
			out[i][j] = inputArray[i*goodValues+j];
			if (out[i][j] != out[i][j]) out[i][j]=0.0;
			if (out[i][j] > 1.0 ) out[i][j]=1.0;
			if (out[i][j] < -1.0 ) out[i][j]=-1.0;
		}
	}
	fprintf(stderr, "done!**\n");
	free(inputArray);

	// Checking matrix integrety before saving
	fprintf(stderr, "Checking matrix integrity before saving ... \n");
	checkMat( out , goodValues );
	fprintf(stderr, "done!**\n");

	fprintf(stderr, "**saving output...");
	FILE *output;
	output = fopen( fout1, "w" ); 
	fprintf( output, "%i \n", goodValues );	
	for(int i = 0 ; i < goodValues ; i++ ) {
		for(int j = i ; j < goodValues ; j++ ) {
			fprintf(output, "%lf \n", out[i][j]);	
		}
	} 
	fprintf(stderr, "done!**\n"); 
	for(int i=0;i<goodValues;i++)free(out[i]);
	free(out);
	fclose(output);
	return 0;
}

