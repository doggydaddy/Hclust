#!/usr/bin/Rscript

# This script takes an input table of cluster statistics,
# a table of threshold values based on cluster CC and size
# and then filters the nodes accordingly

args <- commandArgs(TRUE);

# ARGUMENTS
# 1. table of cluster statistics
# 2. table of the threshold values
# 3. output filtered table
input_table <- read.table(args[1]);
source( args[2] );
output_table <- args[3];

# In bash, initialize the output table file
system(paste0("touch ", output_table));

# formatting the filter table appropriately
filter_table <- data.frame(x=cc,y=pval)
fit <- smooth.spline(cc, pval);

c1_sizes <- input_table[,2];
c1_cc <- input_table[,3];

c2_sizes <- input_table[,4];
c2_cc <- input_table[,5];

c1_cc[is.infinite(c1_cc)] <- 0;
c1_cc[is.infinite(c2_cc)] <- 0;

N <- length(input_table[,1]);
for ( i in 1:N ) {
    if ( c1_sizes[i] > 12 && c2_sizes[i] > 12 ) {
        write(as.numeric(input_table[i,]),file=output_table,append=TRUE,ncolumns=5);
    }
}
