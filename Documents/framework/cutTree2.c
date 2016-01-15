#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <fslio.h>
#include <nifti1_io.h>
//#include <omp.h>
#include "cluster.h"

/* COMMAND: ./cutTree <Tree Name> <Nr. Cuts> <Mask Name> <Output Name> */

int main(int argc, char *argv[]){

	if(!argv[1]) {
		printf("\n./cutTree <Tree Name> <Nr. Cuts> <Mask Name> <Output Name>\n");
		return 0;
	}

	printf("\nLoading the dendrogram ... \n");
	FILE *treeFile = fopen(argv[1], "r");
	int goodValues;
	fscanf(treeFile, "%i", &goodValues);
	Node *tree = malloc((goodValues-1)*sizeof(Node));
    	for(int i=0; i<goodValues-1; i++){
        	fscanf(treeFile, "%i %i %lf", &(tree[i].left), &(tree[i].right), &(tree[i].distance));
   	}
	fclose(treeFile);

	printf("\nCutting the dendrogram ...\n");
        int * clusterid;
        int nr_clusters = atoi(argv[2]);
        clusterid = (int*)malloc(goodValues*sizeof(int));
        cuttree (goodValues, tree, nr_clusters, clusterid);
        free(tree);

	printf("\nLoading mask template ... \n");	
	FSLIO *fslio;
        fslio = FslInit();
        void *buffer = FslReadAllVolumes(fslio, argv[3]);
        double ***mask = FslGetVolumeAsScaledDouble(fslio, 0);
        FslClose(fslio);

	printf("\nSaving the image ...\n");
        nifti_image *nim = nifti_image_read(argv[3], 1);
        int nx = nim->nx; int ny = nim->ny; int nz = nim->nz;
        char *pData = (char *)nim->data;
        int c = 0; int c2 = 0;
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
        nim->data = (char *)pData;
        int ret = nifti_set_filenames(nim, argv[4], 0, 1);
        nifti_image_write( nim );
        nifti_image_free( nim );

	free(clusterid);
        return 0;			
}
