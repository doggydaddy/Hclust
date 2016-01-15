#!/usr/bin/Rscript

args <- commandArgs(TRUE);

a <- list();
# This is the full tree, or dendrogram
den <- read.table( args[1] , skip=1 );

input <- read.table( args[2] );
input <- apply(input,2,rev);
tmp <- input[,3];
input[,3] <- input[,2 ];
input[,2] <- tmp;
n <- length( input[,1] );

a$merge  <- as.matrix(input[,2:3]);
N <- as.numeric( readLines( args[1], n=1 ) );
for ( i in 1:n ) {
	height <- as.numeric(input[i,1]);
	row_num <- N - height ;
	print( row_num );
	a$height[i] <- den[row_num, 3]
}
a$order <- 1:(n+1);
a$labels <- 1:(n+1);
class(a) <- "hclust"
ad <- as.dendrogram(a)
save(ad,file=args[3]);
