#!/usr/bin/Rscript
args <- commandArgs(TRUE);
d <- read.table( args[1] );
d <- sum(d[,1])

