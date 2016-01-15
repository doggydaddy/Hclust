#include <stdio.h>
#include <strings.h>
#include <nifti1.h>
#include <fslio.h>
#include <nifti1_io.h>
#include <math.h>
/*
#include <omp.h>
*/

/* This program extracts CC matrix of a cluster mask given
 input whole CC matrix, mask used to generate said CC matrix,
 and the cluster mask */

/* main function */
int main ( int argc , char * argv[] ) {	
	char *inputMask, *inputSPC, *dataInAsci, *outputFile;
	inputMask = argv[1]; 
	inputSPC = argv[2];
	dataInAsci = argv[3];
	outputFile = argv[4];
	nifti_image *nim_mask = NULL;
	nifti_image *nim_SPC = NULL;

	nim_SPC = nifti_image_read(inputSPC,1);
	int nx=nim_SPC->nx;
	int ny=nim_SPC->ny;
	int nz=nim_SPC->nz;
	int nvox =(int)nim_SPC->nvox;
	short *pSPC=(short *)nim_SPC->data;

	int outSize=0;
	for(int i=0;i<nvox;i++){
		if(pSPC[i]!=0) outSize++;
	}

	nim_mask = nifti_image_read(inputMask,1);
	int sizeOfMask=(int)nim_mask->nvox;
	if(sizeOfMask!=nvox) printf("the sizes are incorrect!\n");
	short *mask=(short *)nim_mask->data;
	int goodValuesTest;
	for(int i=0;i<sizeOfMask;i++){
		if(mask[i]!=0)
			goodValuesTest++;
	}

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
	printf("preparing index:\n");	
	printf("goodValues = %i\n",nvox);
	printf("nvox = %i\n",nvox);
	printf("DEBUG:looping to the data pointers:\n");
	int maskCounter=0;
	for(int i=0;i<nvox;i++){
		if(mask[i]==1) maskCounter++;	
	}
	printf("DEBUG:mask counter = %i\n",maskCounter);
	int pSPCCounter=0;
	for(int i=0;i<nvox;i++){
		if(pSPC[i]==1) pSPCCounter++;	
	}
	printf("DEBUG:pSPC counter = %i\n",pSPCCounter);
	printf("DEBUG:looping complete\n");
	int array[goodValues];
	int counter;
	counter=0;
	printf("starting loop ... \n");
	for ( int i = 0 ; i < nvox ; i++ ) {
		if ( mask[i] != 0 && pSPC[i] != 0) {
			if(counter==goodValues) {
				printf("ERROR!!! index array overloading! exiting ... \n");
				exit(1);
			}
			array[counter] = 1;
			counter++;
		} else if ( mask[i] != 0 && pSPC[i] == 0) {
			if(counter==goodValues) {
				printf("ERROR!!! index array overloading! exiting ... \n");
				exit(1);
			}
			array[counter] = 0;
			counter++;
		} else if ( mask[i] == 0 && pSPC[i] == 0 ) {	
			//printf("case entered=3\n");
		} else if ( mask[i] == 0 && pSPC[i] != 0 ) {
			printf("THE MASKS DO NOT MATCH!!!\n");
			exit(1);
		} else {
			printf("something went horribly wrong!\n");
			exit(1);
		}

	}

	counter = 0;
	for ( int i = 0 ; i < goodValues ; i++ ) {
		if ( array[i] != 0 ) counter++;
	}
	if ( counter != outSize ) printf("counter = %i\n", counter);

	printf("creating output matrix\n");
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
	//printf("saving output\n");
	/*
	const int dims[8]={3,outSize,outSize,1,1,1,1,1};
	nim=nifti_make_new_nim(dims,64,1);
	nim->data=(double*)out;
	int err=nifti_set_filenames(nim,outputFile,0,1);
	nifti_image_write(nim);
	nifti_image_free(nim);
	*/
	printf("done\n");
	FILE *output_asci;
	output_asci = fopen( outputFile, "w" ); 
	fprintf( output_asci, "%i \n", outSize );	
	for(int i = 0 ; i < outSize ; i++ ) {
		for(int j = i ; j < outSize ; j++ ) {
			fprintf(output_asci, "%lf \n", out[i*outSize+j]);	
		}
	} 
	fprintf(stderr, "done!**\n"); 
	free(out);
	fclose(output_asci);
	nifti_image_free(nim_mask);
	nifti_image_free(nim_SPC);
	return 0;
}

