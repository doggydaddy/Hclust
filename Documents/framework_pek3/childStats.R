#!/usr/bin/Rscript
args <- commandArgs(TRUE);

#Reading input
# 1. Cut number index (as obtained from iteration in analyze_childrenAtCut.sh
# 2. Name of the dendrogram (linkdage file from hclust)
# 3. path and name to the full CC matrix
# 4. cutStatsF.txt file to grab cut level info
# 5. output name


cut_number_index <- strtoi( args[1] );
tableF <- read.table( args[4] )
cut_number <- which(tableF==(-1*cut_number_index),arr.ind=T)[1]
cut_number <- tableF[cut_number,1]
importDendrogram <- function( file.name ) {
        data <- read.table( file.name, skip=1 );
        data[,1] <- rev( data[,1] );
        data[,2] <- rev( data[,2] );
        data[,3] <- rev( data[,3] );
        return( data );
}
linkage <- importDendrogram( args[2] );
inconsistency <- function(Z, depth) {
        z <- Z[,3];
        N <- length(z)
        Y <- array( 0 , dim=c(N,4) )
        if ( depth=='full' ) {
                d <- N;
        } else {
                d <- depth;
        }

        for ( i in 1:(N-1) ) {

                if ( i+d > N ) d <- N-i;
                Y[i,1] <- mean( z[i:(i+d)] );
                Y[i,2] <- sd( z[i:(i+d)] );
                Y[i,3] <- d+1;

                if ( Z[i,1]>0 & Z[i,2]>0 ) {
                        Y[i,4] <- 0;
                } else {
                        Y[i,4] <- ( z[i] - Y[i,1] ) / ( Y[i,2] );
                }

        }
        Y <- cbind( 1:N-1 , Y );
        return( Y );
}
IC <- inconsistency( linkage , 'full' );

node_height <- linkage[cut_number,3];
node_ic <- IC[cut_number,5];

cc_cl <- read.table( "inv_cluster.cc" );
# grab size
cl_size <- cc_cl[1,1];
# trim
cc_cl <- abs(cc_cl[-1,1]);

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

if (cl_size == 1) {
	sd_cl <- 0;
	min_cl <- 1;
	mean_cl <- 1;
	var_cl <- 0;
	cl_snr <- 0;
} else {
	cc_cl <- cc_cl[-1*findDiagIndices(cl_size)]

	min_cl <- range(cc_cl)[1];
	mean_cl <- mean(cc_cl);
	sd_cl <- sd(cc_cl);
	var_cl <- var(cc_cl);
	cl_snr <- mean_cl/var_cl;
}

cl_intra_mod <- sum(as.numeric(cc_cl>0.3))
# COMENT AWAY mod calculations because it taks to much time temporarily
# -------------------------------------------------------------------------------
if(FALSE){
cl_intra_mod <- sum(as.numeric(cc_cl>0.3))/((cl_size*(clsize-1))/2)
AVG_CON <- 0.01340502
# Constructing the "full" cc matrix of
cc_matrix <- read.table( args[3] )
gV <- cc_matrix[1,1]
cc_matrix <- cc_matrix[-1,1]
cc_mat <- array(0,dim=c(gV,gV))
c<-1
for ( i in 1:gV ) {
        for (j in i:gV) {
                if( i==j ) {
                        cc_mat[i,j]<-cc_matrix[c]
                } else {
                        cc_mat[i,j]<-cc_matrix[c]
                        cc_mat[j,i]<-cc_mat[i,j]
                }
                c <- c+1
        }
}
cl_spc_mask <- read.table('inv_cluster.dump')
cl_spc_mask <- cl_spc_mask[,1]
n <- sum(cl_spc_mask)
cl_inter_mod <- array(0,dim=(n*(gV-n)))
c<-1;
for ( i in 1:gV ) {
        for ( j in 1:gV ) {
                if (cl_spc_mask[i]==1 && cl_spc_mask[j]==0) {
                        cl_inter_mod[c] <- cc_mat[i,j];
                        c<-c+1;
                }
        }
}
cl_inter_mod <- sum(as.numeric(abs(cl_inter_mod)>0.3))

cl_mod <- cl_intra_mod/cl_inter_mod
}
# -------------------------------------------------------------------------------

options(scipen=999)
r.o <- c(as.numeric(cut_number), node_height, node_ic, cl_size, min_cl, mean_cl, sd_cl, cl_snr, cl_intra_mod);
write(r.o, file=args[5], append=TRUE, ncolumns=9);
