#!/usr/bin/Rscript
args <- commandArgs(TRUE);

input_table <- read.table( args[1] );
N <- length( input_table[,1] );
indexToString <- function( number ) {
        num <- as.numeric(number);
        if ( num >= 0 && num <= 9 ) {
                r.o <- paste0("0000", toString(num));
        }
        else if ( num >= 10 && num <= 99 ) {
                r.o <- paste0("000", toString(num));
        }
        else if ( num >= 100 && num <= 999 ) {
                r.o <- paste0("00", toString(num));
        }
        else if ( num >= 1000 && num <= 9999 ) {
                r.o <- paste0("0", toString(num));
        }
        else if ( num >= 10000 && num <= 99999 ) {
                r.o <- toString(num);
        } else {
                print('index out of bounds!');
                exit( 0 );
        }
        return( r.o );
}


for ( i in 1:N ) {
	xx <- input_table[i,1];
	yy <- input_table[i,2];
	zz <- input_table[i,3];
	print( paste0("DEBUG: Parsing line ",i,": ",xx," ",yy," ",zz) );
	if ( yy < 0 ) {
		yy_abs <- abs( yy );
		system( paste0("3dcalc -a createdChildrenAt",indexToString(xx),".nii -expr '",1,"*within(a,1,1)' -prefix cluster",indexToString(yy_abs),".nii") )
	}
	if ( zz < 0 ) {
		zz_abs <- abs( zz );
		system( paste0("3dcalc -a createdChildrenAt",indexToString(xx),".nii -expr '",1,"*within(a,2,2)' -prefix cluster",indexToString(zz_abs),".nii") )
	}
}

print( "DONE!" );


