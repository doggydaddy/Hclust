#!/bin/bash
# arguments
# 1. mask for full CC matrix
# 2. full CC matrix
# 3. dendrogram on the current hierarchy
# 4. the cutStatsF.txt file to grab cut level info
# 5. Name of the output table (cutStats.txt)

if [[ $# -eq 0 ]] ; then
    echo 'Iterates through all cluster*.nii in folder and computes stats'
    echo 'requires childStats.R'
    echo 'Needs mask for full CC, full CC, dendrogram on CURRENT hierarchy as input, and cutStatsF.txt file'
    echo 'usage: ./clustStats.sh <mask for full CC matrix> <full CC matrix> <dendrogram on cur. hier.> <cutStatF.txt file> <output name>'
    exit 0
fi
touch $5
files=`ls cluster*.nii`
c=1
for i in $files
do
	extractClusterCC $1 $i $2 inv_cluster.cc
	3dmaskdump -mask $1 -noijk -o inv_cluster.dump $i
	childStats.R $c $3 $2 $4 $5
	c=$(($c+1))
	rm inv_cluster.dump inv_cluster.cc
done
