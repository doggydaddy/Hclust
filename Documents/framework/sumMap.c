#include <stdio.h>
#include <stdlib.h> 
#include <strings.h>
#include <math.h>
#include <fslio.h>
#include <nifti1_io.h>
#include <omp.h>

/* This program adds 2 CC matrices together */

int main ( int argc , char * argv[] ) {
	FILE *file;
	char *fin1, *fin2, *fout;
	int goodValues, fsize, access_counter;
	double scanptr;
	double *mat1, *mat2; // distance matrix buffer
	fin1=argv[1]; 
	fin2=argv[2];
	fout=argv[3];
	file = fopen(fin1, "r");
	fscanf(file, "%i", &goodValues );
	fsize = (goodValues*(goodValues+1))/2;
	mat1=(double*)calloc(sizeof(double),fsize);
	for(int i=0; i<fsize; i++) {
		fscanf(file,"%lf",&scanptr);
		mat1[i]=scanptr;
	}
	fclose(file);

	file = fopen(fin2, "r");
	fscanf(file, "%i", &goodValues );
	fsize = (goodValues*(goodValues+1))/2;
	mat2=(double*)calloc(sizeof(double),fsize);
	for(int i=0; i<fsize; i++) {
		fscanf(file,"%lf",&scanptr);
		mat2[i]=scanptr;
	}
	fclose(file);
	
	for(int i=0; i<fsize; i++){
		mat1[i]+=mat2[i];
	}
	free(mat2);
	
	file = fopen( fout, "w" ); 
	fprintf( file, "%i \n", goodValues );	
	for(int i = 0 ; i < fsize ; i++ ) {
		fprintf(file, "%lf \n", mat1[i]);	
	} 
	fprintf(stderr, "done!**\n"); 
	fclose(file);
	free(mat1);
	return 0;
}

