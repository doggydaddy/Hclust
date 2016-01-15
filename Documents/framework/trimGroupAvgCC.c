#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

/* This program trims input CC matrx in upper triangular format
 to two files, one positive values and other negative. It also
 checks for [-1,1] bounds and corrects them when necessary */

/* The out of bounds correction has already been done in calcCC 
 so it isn't really necessary here */

int main(int argc,char *argv[]){
	FILE *fin, *foutPos, *foutNeg;
	char *finName, *foutPosName, *foutNegName;
	finName=argv[1];
	foutPosName=argv[2];
	foutNegName=argv[3];
	fin=fopen(finName,"r");
	int goodValues;
	fscanf(fin,"%i",&goodValues);
	int fsize=(goodValues*(goodValues+1))/2;
	printf("scanning input file ...\n");
	int c=0;
	double d;
	foutPos=fopen(foutPosName,"w");
	foutNeg=fopen(foutNegName,"w");
	fprintf(foutPos,"%i\n",goodValues);
	fprintf(foutNeg,"%i\n",goodValues);
	int correctedPositives, correctedNegatives;
	for(int i=0;i<fsize;i++){
		fscanf(fin,"%lf",&d);
		// correct for >1 double precision errors
		if(d>1.0){
			d=1.0;
			correctedPositives++;
		}
		if(d<-1.0){
			d=-1.0;
			correctedNegatives++;
		}

		if(d>=0.0){
			fprintf(foutPos,"%lf \n",d);
			fprintf(foutNeg,"%lf \n",0.0);	
		}else{
			d=-1*d;
			fprintf(foutNeg,"%lf \n",d);
			fprintf(foutPos,"%lf \n",0.0);	
		}
	}
	fclose(fin); fclose(foutPos); fclose(foutNeg);
	return 0;
}
