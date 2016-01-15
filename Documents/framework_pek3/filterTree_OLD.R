#!/usr/bin/Rscript
args <- commandArgs(TRUE);

input_table <- read.table(args[1]);
data_plot <- source( args[2] );
output_table <- args[3];
system(paste0("touch ", output_table));

dat <- data.frame(x=cc,y=p99)
fit <- nls(y ~ I(exp(1)^(a + b * x)), data=dat, start = list(a=0,b=0))
fitted_a <- summary(fit)$coefficients[1,1]
fitted_b <- summary(fit)$coefficients[2,1]
fp99 <- function ( x , a = fitted_a , b = fitted_b ) 
{
	return( I(exp(1)^(a + b * x)) )
}

N <- length(input_table[,1]);

c1_sizes <- input_table[,2];
c1_cc <- input_table[,3];
c2_sizes <- input_table[,6];
c2_cc <- input_table[,7];

c1_threshold_list <- fp99(c1_cc) <  c1_sizes;
c2_threshold_list <- fp99(c2_cc) <  c2_sizes;

for ( i in 1:N ) {
	if ( c1_sizes[i] > 23 && c2_sizes[i] > 23 ) {
		if ( c1_threshold_list[i]==TRUE && c2_threshold_list[i]==TRUE ) {
			write(as.numeric(input_table[i,]),file=output_table,append=TRUE,ncolumns=9);
		}
	}
}
