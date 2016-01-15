#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//#include <omp.h>

/* This Program thresholds a CC matrix */

/* This program checks for NaNs and 
 truncates them to an arbitrarily large value.
 Also performs threshold on a given value to which
 any distances above that is truncates to 1 */

int main ( int argc , char * argv[] ) {	

	int nanValues = 0; int truncatedValues = 0;
	double cc;

	// input and output file name strings
	char *fin_name, *fout_name, *cc_thresh; 
	fin_name = argv[1];	
	fout_name = argv[2];
	cc_thresh = argv[3];
	double cct = atof(cc_thresh);
	
	FILE * fin = fopen ( fin_name , "r" );
	/* grabbing dimensions */

	int goodValues, fsize, access_counter; 
	fscanf( fin, "%i", &goodValues );
	fsize = ( goodValues * (goodValues + 1) ) / 2;
	/* reading from input file */
	fprintf( stderr , "** scanning input file ... " );
	access_counter = 0; 

	FILE * fout = fopen( fout_name , "w" );
	fprintf( fout , "%i \n", goodValues );

	for ( int i = 0 ; i < fsize ; i++ ) {
		fscanf( fin, "%lf", &cc);
		cc = fabs(cc);
		// checking for bad values 
		if ( cc != cc ) {
			cc = 1337.0000;
			nanValues++;	
		}
		// Thresholding
		else if ( cc < cct  ) {
			cc = 0;
			truncatedValues++;
		}
		fprintf( fout , "%f \n", cc );
	}
	fclose( fin );	
	return 0;
}
