#include <stdio.h>
#include <strings.h>
#include <nifti1.h>
#include <fslio.h>
#include <nifti1_io.h>
#include <math.h>
#include <omp.h>

/* this programs takes input data CC matrx, mask used to generate CC matrx
 and a cluster mask and outputs simple statistics of the cluster */

/* aid functions */

/* this method calculates each voxels average
	from this average voxels 1dim array,
	calculates the simple stats of the cluster
	@param: input = full input matrix to be processed
	@param: dim = height and width  of the input matrix,
	this method assumes the matrix is square.
	@param: integer flag for batch mode output or not. */
void clusterStat(double *input, int dim, int batchModeFlag){
	double avgValues[dim];
	int i,j,c;
	double currVoxSum, min, max, avgVoxAvg;
	double std, pew, var;
	/* calculating the average voxel connections */
	for(i=0;i<dim;i++){
		currVoxSum=0.0;
		for(j=0;j<dim;j++){
			currVoxSum+=input[i*dim+j];	
		}		
		avgValues[i]=currVoxSum/dim;
	}
	max=0;min=1;
	/* doing the simple stat. thing for avgValues array */
	for(i=0;i<dim;i++){
		if(avgValues[i]>max) max=avgValues[i];
		if(avgValues[i]<min) min=avgValues[i];
		avgVoxAvg+=avgValues[i];
	}
	avgVoxAvg/=dim;
	for(i=0;i<dim;i++){
		pew+=pow(avgValues[i]-avgVoxAvg,2);
	}
	var=pew/dim;
	std=sqrt(var);
	if(batchModeFlag==1){
		printf("%lf %lf %lf %lf %lf",min,max,avgVoxAvg,var,std);
	} else {
		printf("clusterStat Says: \n");
		printf("min/mas=%lf/%lf\n",min,max);
		printf("average=%lf\n",avgVoxAvg);
		printf("VAR/STD=%lf/%lf\n",var,std);
	}	
}
/* main function */
int main ( int argc , char * argv[] ) {	
	char *inputMask, *inputSPC, *dataInAsci;
	inputMask = argv[1]; 
	inputSPC = argv[2];
	dataInAsci = argv[3];
	nifti_image *nim = NULL;
	nim = nifti_image_read(inputSPC,1);
	int nx=nim->nx;
	int ny=nim->ny;
	int nz=nim->nz;
	int nvox =(int)nim->nvox;
	short *pSPC=(short *)nim->data;
	int outSize=0;
	for(int i=0;i<nvox;i++){
		if(pSPC[i]!=0)
			outSize++;
	}
	nim = nifti_image_read(inputMask,1);
	int sizeOfMask=(int)nim->nvox;
	if(sizeOfMask!=nvox) printf("the sizes are correct!\n");
	short *mask=(short *)nim->data;
	int goodValuesTest;
	for(int i=0;i<sizeOfMask;i++){
		if(mask[i]!=0)
			goodValuesTest++;
	}
	nifti_image_free(nim);
	FILE *file;
	file = fopen ( dataInAsci , "r" );
	int goodValues;
	fscanf( file, "%i", &goodValues );
	int fsize;
	fsize = (goodValues * (goodValues+1)) / 2;
	int access_counter = 0; 
	double **mat;
	mat = (double**)malloc(goodValues*sizeof(double*));	
	for ( int i = 0 ; i < goodValues ; i++ ) 
		mat[i] = (double*)malloc(goodValues*sizeof(double));	
	double d;
	for ( int i = 0 ; i < goodValues ; i++ ) {
		for ( int j = i ; j < goodValues ; j++ ) {
			fscanf( file, "%lf", &d);
			if ( i == j )
				mat[i][j] = d;	
			else {
				mat[i][j] = d;
				mat[j][i] = mat[i][j];
			}
		}
	}
	fclose( file );	
	int array[goodValues];
	int counter;
	counter=0;
	for ( int i = 0 ; i < nvox ; i++ ) {
		if ( mask[i] != 0 && pSPC[i] != 0) {
			array[counter] = 1;
			counter++;
		} else if ( mask[i] != 0 && pSPC[i] == 0) {
			array[counter] = 0;
			counter++;
		} else if ( mask[i] == 0 && pSPC[i] != 0 ) {
			printf("THE MASKS DO NOT MATCH!!!\n");
		} else if ( mask[i] == 0 && pSPC[i] == 0 ) {	
		} else {
			printf("something went horribly wrong!\n");
		}
	}
	counter = 0;
	for ( int i = 0 ; i < goodValues ; i++ ) {
		if ( array[i] != 0 ) counter++;
	}
	if ( counter != outSize ) printf("counter = %i\n", counter);
	//printf("creating output matrix\n");
	double *out;
	out = (double*)calloc(outSize*outSize,sizeof(double));
	int oCount = 0;
	for(int i = 0; i < goodValues ; i++ ) {
		for(int j = 0; j < goodValues ; j++ ) {
			if ( array[i] == 1 && array[j] == 1 ) {
				out[oCount]=mat[i][j];
				oCount++;
				if(oCount == outSize*outSize+1) 
					printf("indexing wrong! abort! \n");
			}
		}
	}
	for(int i=0;i<goodValues;i++) free(mat[i]);
	free(mat);
	clusterStat(out,outSize,1);
	free(out);
	return 0;
}

