#!/bin/bash
gcc -ansi -std=gnu99 -pedantic -fopenmp -std=c99 -o $1 $1.c -I$HOME/nifticlib-2.0.0/include -I$PWD -L$HOME/nifticlib-2.0.0/lib -L$PWD -lfslio -lniftiio -lznz -lm -lz


