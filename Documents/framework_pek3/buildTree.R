#!/usr/bin/Rscript
args <- commandArgs( TRUE );

input_table <- read.table( args[1] );
N <- length( input_table[,1] );

output_table <- array( 0, dim=c(N,3) )

output_table[,1] <- input_table[,1]

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
paternityTest <- function ( cut_level , tab ) {
	n <- cut_level - 1;
	flag <- 0;
	while ( n > 0 ) {
		if ( flag != 0 ) { 
			print ( paste0("DEBUG: paternityTest flag is ",flag,". Parent found! breaking at ",n,"!!!") );
			break; 
		} else {
			command_string <- paste0("findMother.sh createdChildrenAt",indexToString(as.numeric(tab[cut_level,1])),".nii createdChildrenAt",indexToString(as.numeric(tab[n,1])),".nii");
			print( paste0("DEBUG: running paternityTest : ", command_string) );
			flag <- as.numeric( system(command_string, intern=TRUE) );
			n <- n - 1;
		}
	}
	print ( "DEBUG: paternityTest is:" );
	print ( c(n + 1,flag) );
	return( c(n + 1,flag) );
}
updateLeaves <- function ( tab ) {
	N <- length( tab[,1] );
	leaf_index <- -1;
	for( i in seq(N,1,-1) ) {
		for ( j in seq(3,2,-1) ) {
			if ( tab[i,j] <= -1 ) {
				tab[i,j] <- leaf_index;
				leaf_index <- leaf_index - 1;
			}
		}
	}
	print( "DEBUG updateLeaves" );
	print( leaf_index );
	return( tab );
}

kounter <- -1;
for ( i in 1:N ) {
	print( paste0("DEBUG: Processing line ",i, ".") );
	if ( i == 1 ) {
		output_table[i,2]<-kounter;		
		kounter <- kounter - 1;
		output_table[i,3]<-kounter;		
		kounter <- kounter - 1;
	}
	else {
		wasCutFromIndex <- paternityTest(i, output_table );
		if ( wasCutFromIndex[1] == 0 && wasCutFromIndex[2] == 0 ) {
			print( "DEBUG: no parent was found" );
			output_table[i,2]<-kounter;		
			kounter <- kounter - 1;
			output_table[i,3]<-kounter;		
			kounter <- kounter - 1;
		} else {
			print( "DEBUG: parent was found" );
			output_table[i,2]<-kounter;		
			kounter <- kounter - 1;
			output_table[i,3]<-kounter;		
			kounter <- kounter - 1;

			if ( output_table[wasCutFromIndex[1],wasCutFromIndex[2]+1] > 0 ) {
				print( "ERROR!!!, trying to attach to an existing parent! abort abort!" );
				exit(0);
			}	
			output_table[wasCutFromIndex[1],wasCutFromIndex[2]+1] <- N-i+1;
			output_table <- updateLeaves( output_table );
		}
	}
}
write.table(output_table, file=args[2], row.names=FALSE, col.names=FALSE, quote=FALSE)

