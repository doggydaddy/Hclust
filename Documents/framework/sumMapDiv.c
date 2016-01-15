#include <stdio.h>
#include <stdlib.h> 
#include <strings.h>
#include <math.h>
#include <fslio.h>
#include <nifti1_io.h>
#include <omp.h>

/* This program divides a sum CC matrix (group data) with
 a specified number (the num. of datasets in the sum CC matrix.
 This program is thought to follow sumMaps program and is a part
 of the pipeline */

int main ( int argc , char * argv[] ) {
	FILE *file;
	char *fin, *fout, *number;
	int goodValues, fsize, access_counter;
	double scanptr;
	double *mat; // distance matrix buffer
	fin=argv[1]; 
	fout=argv[2];
	number=argv[3];
	int num = atoi(number);

	file = fopen(fin, "r");
	fscanf(file, "%i", &goodValues );
	fsize = (goodValues*(goodValues+1))/2;
	mat=(double*)calloc(sizeof(double),fsize);

	for(int i=0; i<fsize; i++) {
		fscanf(file,"%lf",&scanptr);
		mat[i]=scanptr;
	}
	fclose(file);
	
	for(int i=0; i<fsize; i++){
		mat[i]/=num;
	}
	
	file = fopen( fout, "w" ); 
	fprintf( file, "%i \n", goodValues );	
	for(int i = 0 ; i < fsize ; i++ ) {
		fprintf(file, "%lf \n", mat[i]);	
	} 
	fprintf(stderr, "done!**\n"); 
	fclose(file);
	free(mat);
	return 0;
}

