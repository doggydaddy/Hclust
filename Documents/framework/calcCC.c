#include <stdio.h>
#include <strings.h>
#include <nifti1.h>
#include <fslio.h>
#include <nifti1_io.h>
#include <math.h>
#include <omp.h>

/* This program takes a preprocessed fMRI dataset
 and a mask and computes the CC values of all voxels
 in that mask of the dataset and exports it as an asci
 upper triangular CC matrix file.

 This program automatically checks for out of bounds and NaNs

 This program is THE standard program in the pipeline */

/*functions*/
/* retrieves a time series to @output from a 4D dataset @data
   given the spatial coordinates (x,y,z)
   The time series extracted is from index 0-@t */
void getTimeSeries(double ****data, double * output, int x, int y, int z, int t) {
	for ( int i = 0 ; i < t ; i++ ) {
		output[i] = data[i][z][y][x];	
	}
}
int findEffectiveCount(double ***mask, int nx, int ny, int nz) {
	int count = 0;
	for ( int i = 0 ; i < nx ; i++ ) {
	for ( int j = 0 ; j < ny ; j++ ) {
	for ( int k = 0 ; k < nz ; k++ ) {
		if ( mask[k][j][i] != 0 ) {
			count++;
		}
	}}}
	return count;
}
void getNthSeries(double *data, double *output, int n, int nrVoxels, int time) {
	int c = 0;
	for ( int i = n*time ; i < n*time+time ; i++ ) {
		output[c] = data[i];
		c++;
	} 
}
/* Calculate the cross correlation between two time series
   of voxels with coordinates as input at given delay */ 
double calcCrossCorr(double *data, int n, int m, int goodValues, int t) 
{
	double t1[t]; 
	double t2[t];
	getNthSeries(data,t1,n,goodValues,t);
	getNthSeries(data,t2,m,goodValues,t);
	double m1 = 0; double m2 = 0;
	for ( int i = 0; i < t; i++ ) {
		m1 += t1[i];
		m2 += t2[i];
	}
	m1 /= t; 
	m2 /= t;	
	double nom = 0; double de1 = 0; double de2 = 0;
	for ( int i = 0 ; i < t ; i++ ) {
		nom += (t1[i] - m1) * (t2[i] - m2); 	
		de1 += (t1[i] - m1) * (t1[i] - m1);  
		de2 += (t2[i] - m2) * (t2[i] - m2);
	}
	double output;
	output = nom / ( sqrt(de1*de2) ); 
	return output;
}
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
                        if ( matrix[i][j] != matrix[i][j] ) {
                                matrix[i][j] = 1;
                                nans++;
                        }
			else if ( matrix[i][j] == 1 ) {
				maxs++;
			}
			else if ( matrix[i][j] == 0 ) {
				zeros++;
			}
                }
        }
        if ( nans != 0 ) {
		pnan = (nans*100)/total;
                printf("DEBUG: NaN values = %i (%.3lf) \n",nans,pnan);
        }
        if ( maxs != 0 ) {
		pmax = (maxs*100)/total;
                printf("DEBUG: max distances = %i (%.3lf) \n",maxs,pmax);
        }
        if ( zeros != 0 ) {
		pzeros = (maxs*100)/total;
                printf("DEBUG: exactly zeros = %i (%.3lf) \n",zeros,pzeros);
        }
}
/* main function */
int main ( int argc , char * argv[] ) {
	FILE *output;
	char *inputFile, *maskFile, *fout1, *fout2, *fout3;
	FSLIO *fslio;
	void *buffer;
	double ****data, ***mask;
	int nx, ny, nz, nt, maxDelay;
	int goodValues, fsize;
	double *mat, **out, **out_nd;
	int **del;
	long c1, c2, c3;
	int err, atDelay, atDelay2;
	double corr, corr2, corr_nd, corr2_nd;
	/* parsing input params */
	inputFile = argv[1]; maskFile = argv[2];
	fout1 = argv[3];
	/* loading datset and mask */	
	// mask	
	fslio = FslInit(); 
	buffer = FslReadAllVolumes(fslio, maskFile);
	mask = FslGetVolumeAsScaledDouble(fslio,0);
	if ( mask == NULL ) { printf("error loading mask!\n"); exit(1); }
	err = FslClose(fslio);
	// dataset
	fslio = FslInit(); 
	buffer = FslReadAllVolumes(fslio, inputFile);	
	data = FslGetBufferAsScaledDouble(fslio);
	if ( data == NULL ) { printf("error loading data!\n"); exit(1); }
	err = FslClose(fslio);
	/* grabbing dimensions of the data */
	nx = fslio->niftiptr->nx; ny = fslio->niftiptr->ny;
	nz = fslio->niftiptr->nz; nt = fslio->niftiptr->nt; 
	/* calculate sizes */
	goodValues = findEffectiveCount(mask, nx, ny, nz);
	fsize = (goodValues*(goodValues+1))/2;

	fprintf(stderr, "Allocating some memory... ");	
	mat = (double*)calloc(goodValues*nt,sizeof(double));
	out = (double**)calloc(goodValues,sizeof(double*));
	for(int i = 0; i < goodValues; i++ ) out[i] = (double*)calloc(goodValues,sizeof(double));
	fprintf(stderr, "...DONE!\n");	
	
	fprintf(stderr,"starting transfer...");
	c1 = 0;
	#pragma omp for ordered schedule(dynamic)
	for ( int k = 0 ; k < nz ; k++ ) {
	for ( int j = 0 ; j < ny ; j++ ) {	
	for ( int i = 0 ; i < nx ; i++ ) {
	for ( int l = 0 ; l < nt ; l++ ) {	
		if ( mask[k][j][i] != 0 ) {
			mat[c1] = data[l][k][j][i];
			c1++;
		}	
	}}}}
	fprintf(stderr,"complete!\n");	
	fprintf(stderr, "**performing cc calculations...\n");
	#pragma omp parallel for
	for ( int i = 0 ; i < goodValues ; i++ ) {
		for ( int j = i ; j < goodValues ; j++ ) {
			out[i][j] = calcCrossCorr(mat,i,j,goodValues,nt);
			if (out[i][j] != out[i][j]) out[i][j]=0.0;
			if (out[i][j] > 1.0 ) out[i][j]=1.0;
			if (out[i][j] < -1.0 ) out[i][j]=-1.0;
			//if (out[i][j] > 0.6) out[i][j]=1;
		}
	}
	fprintf(stderr, "done!**\n");
	free(mat);
	//checkMat(out,goodValues);
	fprintf(stderr, "**saving output...");
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

