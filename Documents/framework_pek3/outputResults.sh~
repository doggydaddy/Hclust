#!/bin/bash

# This is a wrapper script to prepare full dendrogram cutting reulsts up to a
# certain cut level.
# This script requires <cutTree> <findCut.sh>
# danger words are: atCut_*.nii createdChildrenAt*.nii
# Program assumes toCutNumber is between 2 and 99999 (should be sufficient for most cases)


# usage: ./outputResults.sh <dendrogram name> <dendrogram mask> <to cut number>
# <output name (cluster results)> <output name (children at cuts)>

# Loading arguments
dendrogramName=$1
dendrogramMask=$2
toCutNumber=$3
outputCluster=$4
outputAtCutCluster=$5

# Performing Cut
for ((i=1; i<=$toCutNumber; i++))
do
	if [ $i ge 10000 ] && [ $i -le 99999 ]; then
		cutTree $dendrogramName $i $dendrogramMask cutAt_$i.nii
	elif [ $i -ge 1000 ] && [ $i -le 9999 ]; then
		cutTree $dendrogramName $i $dendrogramMask cutAt_0$i.nii
	elif [ $i -ge 100 ] && [ $i -le 999 ]; then
		cutTree $dendrogramName $i $dendrogramMask cutAt_00$i.nii
	elif [ $i -ge 10 ] && [ $i -le 99 ]; then
		cutTree $dendrogramName $i $dendrogramMask cutAt_000$i.nii
	elif [ $i -ge 1 ] && [ $i -le 9 ]; then
		cutTree $dendrogramName $i $dendrogramMask cutAt_0000$i.nii
	fi
done
3dbucket -prefix $outputCluster cutAt_*.nii
rm cutAt_*.nii
for (( i=1; i<=$(($toCutNumber-1)); i++ ))
do
	findCut.sh $outputCluster $i
done
3dbucket -prefix $outputAtCutCluster createdChildrenAt*.nii


