#include <stdio.h>
#include <stdlib.h> 
#include <strings.h>
#include <math.h>
#include <fslio.h>
#include <nifti1_io.h>

//#include <omp.h>

#include "cluster.h" /* The C Clustering Library */

int main ( int argc , char * argv[] ) {
	FILE *file;
	char *fout, *fin, *fmask;
	void *buffer;
	double ***mask;
	nifti_image * nim = NULL; 
	int goodValues, fsize, access_counter;
	double scanptr;
	double *mat; // distance matrix buffer
	double **dist; // distance matrix	
	fin=argv[1]; 
	fmask=argv[2];
	fout=argv[3];
	printf("read matrix\n");
	file = fopen(fin, "r");
	fscanf(file, "%i", &goodValues );
	fsize = (goodValues*(goodValues+1))/2;
	mat = (double*)calloc(sizeof(double),fsize);
	for(int i=0; i<fsize; i++) {
		fscanf(file,"%lf",&scanptr);
		mat[i]=scanptr;
	}
	fclose(file);
	printf("formatting read matrix\n");
	dist=(double**)calloc(sizeof(double*),goodValues);
	for(int i=0; i<goodValues; i++) {
		dist[i]=(double*)calloc(sizeof(double),goodValues);
	}
	int c=0;
	for(int i=0; i<goodValues; i++){
		for(int j=i; j<goodValues; j++){
			dist[i][j]=mat[c];
			c++;
			if(i==j) dist[i][j]=0;
			if(i!=j) dist[j][i]=dist[i][j];
		}
	}
	/* HIERARCHICAL CLUSTERING */	
	printf("clust\n");
	Node * tree;
	tree = treecluster(goodValues, goodValues, 0, 0, 0, 0, 'a', 'a', dist);	
  	if (!tree) { /* Indication that the treecluster routine failed */
    		fprintf (stderr,"treecluster routine failed due to insufficient memory\n");
    		return 1;
  	}
	printf("Saving tree ... \n");
	int ltree = sizeof(&tree) / sizeof(tree[0]);
	printf("debug: ltree = %i\n",ltree);
	FILE * treeFile = fopen("saved_tree","w");
	fprintf(treeFile,"%i\n",goodValues);
	for(int i=0; i<goodValues-1; i++){
		fprintf(treeFile,"%i %i %lf\n",tree[i].left, tree[i].right, tree[i].distance);
	}
	fclose(treeFile);

	fprintf(stderr,"** Cutting tree ...\n");
  	int * clusterid;
	int nr_clusters = atoi(argv[4]);
	clusterid = (int*)malloc(goodValues*sizeof(int));
  	cuttree (goodValues, tree, nr_clusters, clusterid);
	
	free(tree);	

	printf("Saving images ... \n");
	FSLIO *fslio;	
	fslio = FslInit();
	buffer = FslReadAllVolumes(fslio, fmask);
	mask = FslGetVolumeAsScaledDouble(fslio, 0);	
	FslClose(fslio);
	nim = nifti_image_read(fmask, 1);	
	int nx = nim->nx;
	int ny = nim->ny;
	int nz = nim->nz;
	short *pData = (short *)nim->data;
	c = 0; int c2 = 0;
	for(int k=0;k<nz;k++) {
	for(int j=0;j<ny;j++) {
	for(int i=0;i<nx;i++) {
		if ( mask[k][j][i] != 0 ) {
			pData[c2] = clusterid[c]+1;
			c++; c2++;
		}
		else {
			pData[c2] = 0;
			c2++;
		}
	}}}		
	nim->data = (short *)pData;		
	int ret = nifti_set_filenames(nim, fout, 0, 1);
	nifti_image_write( nim );	
	nifti_image_free( nim );	
	return 0;
}

