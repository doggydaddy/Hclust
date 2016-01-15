#!/usr/bin/Rscript
# quick fixerupper for the last row

args <- commandArgs(T);

table <- read.table(args[1]);

N <- length(table[,1]);

table[N,1]<-table[N-1,1]
table[N,2]<-table[N-1,2]
table[N,3]<-table[N-1,3]

colnames(table) <- c("k rel.","height","IC","cl. size","avg. cc","cl snr","mod");

write.table(table,file=args[2],row.names=FALSE,quote=FALSE)
