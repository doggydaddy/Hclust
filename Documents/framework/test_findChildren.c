#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <fslio.h>
#include <nifti1_io.h>
//#include <omp.h>
#include "cluster.h"

/* COMMAND: ./findChildren <Tree Name> <Nr. Number> <Mask Name> <Output Name> */

void dendSearch( int line_nr, Node *tree, int* return_object, int goodValues ) {
	if ( tree[goodValues-line_nr].left >= 0 ) {
		return_object[tree[line_nr].left] = 1; 	
	} else {
		dendSearch( abs(tree[goodValues-line_nr].left), tree, return_object, goodValues );
	}	
	if ( tree[goodValues-line_nr].right >= 0 ) {
		return_object[tree[line_nr].right] = 1; 	
	} else {
		dendSearch( abs(tree[goodValues-line_nr].right), tree, return_object, goodValues );
	}	
}

int main(int argc, char *argv[]){

	if(!argv[1]) {
		printf("\n./findChildren <Tree Name> <Nr. Cuts> <Mask Name> <Output Name>\n");
		return 0;
	}

	printf("\nLoading the dendrogram ... \n");
	FILE *treeFile = fopen(argv[1], "r");
	int goodValues;
	fscanf(treeFile, "%i", &goodValues);
	float tree[goodValues][3]
    	for(int i=0; i<goodValues-1; i++){
        	fscanf(treeFile, "%i %i %lf", &(tree[i][1]), &(tree[i][2]), &(tree[i][3]));
   	}
	fclose(treeFile);

	printf("\nSearching the dendrogram ...\n");
        int cutNr = atoi(argv[2]);
        int *clusterid = (int*)malloc(goodValues*sizeof(int));
	int leftChild = curNode.left;
	dendSearch( leftChild, tree, clusterid, goodValues);
	dendSearch( rightChild, tree, clusterid2, goodValues);
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
        short *pData = (short *)nim->data;
        int c = 0; int c2 = 0;
        for(int k=0;k<nz;k++) {
        for(int j=0;j<ny;j++) {
        for(int i=0;i<nx;i++) {
                if ( mask[k][j][i] != 0 ) {
                        pData[c2] = clusterid[c];
                        c++; c2++;
                }
                else {
                        pData[c2] = 0;
                        c2++;
                }
        }}}
        nim->data = (short *)pData;
        int ret = nifti_set_filenames(nim, argv[4], 0, 1);
        nifti_image_write( nim );
        nifti_image_free( nim );

	free(clusterid);
	free(clusterid2);
        return 0;			
}
