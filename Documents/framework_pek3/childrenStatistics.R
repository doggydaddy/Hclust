#!/usr/bin/Rscript
args <- commandArgs(TRUE);
cc_c1 <- read.table( "firstChild.cc" );
cc_c2 <- read.table( "secondChild.cc" );
# grab sizes
c1_size <- cc_c1[1,1];
c2_size <- cc_c2[1,1];
# trim
cc_c1 <- abs(cc_c1[-1,1]);
cc_c2 <- abs(cc_c2[-1,1]);

findDiagIndices <- function ( goodValues ) {
	cl_size <- goodValues;
	diag_indices <- array(0,dim=cl_size)
	c <- -1
	k <- 1;
	for( i in 1:cl_size ) {
		if ( i == 1 ) {
			diag_indices[i]<-1;
		} else {
			k <- k + cl_size - (i-2);
			diag_indices[i]<- k;
		}
	}
	return( diag_indices );
}

cc_c1 <- cc_c1[-1*findDiagIndices(c1_size)]
cc_c2 <- cc_c2[-1*findDiagIndices(c2_size)]

min_c1 <- min(cc_c1);
min_c2 <- min(cc_c2);

options(scipen=999)
r.o <- c(strtoi(args[1]), c1_size, min_c1, c2_size, min_c2);
write(r.o, file="cutStats.txt", append=TRUE, ncolumns=5);
