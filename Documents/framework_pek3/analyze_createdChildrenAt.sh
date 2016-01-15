#!/bin/bash
files=`ls createdChildrenAt*`
c=1
for i in $files
do
	#grabbing first child
	3dcalc -a $i -expr "within(a,1,1)" -prefix firstChild.nii
	#grabbing second child
	3dcalc -a $i -expr "within(a,2,2)" -prefix secondChild.nii
	extractClusterCC $1 firstChild.nii $2 firstChild.cc
	extractClusterCC $1 secondChild.nii $2 secondChild.cc
	childrenStatistics.R $c
	c=$(($c+1))
	rm firstChild.nii secondChild.nii firstChild.cc secondChild.cc
done
