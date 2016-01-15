#!/bin/bash

#
# This script finds the cluster being divided and what it is being dividied
# into at the next cut. 
#
# Outputs a *.nii file of a the clusters 2 children.
# Name of output file: createdChildrenAt<cut number>.nii
#
# Usage: ./<findCut> <inputFile> <cut number>
#

bucketFile=$1
cutNum=$2

motherIndex=$(($cutNum-1))
childrenIndex=$cutNum

3dbucket -prefix motherCutClusters.nii $bucketFile[$motherIndex] >/dev/null 2>&1
3dbucket -prefix childCutClusters.nii $bucketFile[$childrenIndex] >/dev/null 2>&1

for (( i=1 ; i<=$(($motherIndex+1)); i++ ))
do
	if [ $i ge 10000 ] && [ $i -le 99999 ]; then
		3dcalc -a motherCutClusters.nii -expr "within(a,$i,$i)" -prefix motherCluster$i.nii >/dev/null 2>&1
	elif [ $i -ge 1000 ] && [ $i -le 9999 ]; then
		3dcalc -a motherCutClusters.nii -expr "within(a,$i,$i)" -prefix motherCluster0$i.nii >/dev/null 2>&1
	elif [ $i -ge 100 ] && [ $i -le 999 ]; then
		3dcalc -a motherCutClusters.nii -expr "within(a,$i,$i)" -prefix motherCluster00$i.nii >/dev/null 2>&1
	elif [ $i -ge 10 ] && [ $i -le 99 ]; then
		3dcalc -a motherCutClusters.nii -expr "within(a,$i,$i)" -prefix motherCluster000$i.nii >/dev/null 2>&1
	elif [ $i -ge 1 ] && [ $i -le 9 ]; then
		3dcalc -a motherCutClusters.nii -expr "within(a,$i,$i)" -prefix motherCluster0000$i.nii >/dev/null 2>&1
	fi
done
for (( i=1 ; i<=$(($childrenIndex+1)); i++ ))
do
	if [ $i ge 10000 ] && [ $i -le 99999 ]; then
		3dcalc -a childCutClusters.nii -expr "within(a,$i,$i)" -prefix childCluster$i.nii >/dev/null 2>&1
	elif [ $i -ge 1000 ] && [ $i -le 9999 ]; then
		3dcalc -a childCutClusters.nii -expr "within(a,$i,$i)" -prefix childCluster0$i.nii >/dev/null 2>&1
	elif [ $i -ge 100 ] && [ $i -le 999 ]; then
		3dcalc -a childCutClusters.nii -expr "within(a,$i,$i)" -prefix childCluster00$i.nii >/dev/null 2>&1
	elif [ $i -ge 10 ] && [ $i -le 99 ]; then
		3dcalc -a childCutClusters.nii -expr "within(a,$i,$i)" -prefix childCluster000$i.nii >/dev/null 2>&1
	elif [ $i -ge 1 ] && [ $i -le 9 ]; then
		3dcalc -a childCutClusters.nii -expr "within(a,$i,$i)" -prefix childCluster0000$i.nii >/dev/null 2>&1
	fi	
done

motherClusters=`ls motherCluster*.nii`
childClusters=`ls childCluster*.nii`

echo "starting search"
for mother in $motherClusters
do
	echo "performing children search for mother $mother"
	for child in $childClusters
	do
		3dcalc -a $mother -b $child -expr 'and(a,b)' -prefix intersect.nii >/dev/null 2>&1

		3dmaskave intersect.nii &> maskave_dump
		awk '!/+/' maskave_dump > res	
		rm maskave_dump

                result=`less res`
                set -- $result
                if [ "$1" != "0" ]
                then
                        echo "$child" >> tmpLog
                fi
                rm intersect.nii
                rm res

	done

	counter=`wc -l "tmpLog" | awk '{print $1}'`
	if [[ "$counter" -eq "2" ]]
	then
		echo "found! mother search terminating."
		echo "`less tmpLog`" >> outputLog 
		rm tmpLog
		break 1
	fi
	rm tmpLog
done
child1=`sed -n 1,1p outputLog`
child2=`sed -n 2,2p outputLog`

if [ $cutNum ge 10000 ] && [ $cutNum -le 99999 ]; then
	3dcalc -a $child1 -b $child2 -expr '(a*2)+b' -prefix createdChildrenAt$cutNum.nii >/dev/null 2>&1
elif [ $cutNum -ge 1000 ] && [ $cutNum -le 9999 ]; then
	3dcalc -a $child1 -b $child2 -expr '(a*2)+b' -prefix createdChildrenAt0$cutNum.nii >/dev/null 2>&1
elif [ $cutNum -ge 100 ] && [ $cutNum -le 999 ]; then
	3dcalc -a $child1 -b $child2 -expr '(a*2)+b' -prefix createdChildrenAt00$cutNum.nii >/dev/null 2>&1
elif [ $cutNum -ge 10 ] && [ $cutNum -le 99 ]; then
	3dcalc -a $child1 -b $child2 -expr '(a*2)+b' -prefix createdChildrenAt000$cutNum.nii >/dev/null 2>&1
elif [ $cutNum -ge 1 ] && [ $cutNum -le 9 ]; then
	3dcalc -a $child1 -b $child2 -expr '(a*2)+b' -prefix createdChildrenAt0000$cutNum.nii >/dev/null 2>&1
fi


rm motherCluster*.nii 
rm childCluster*.nii
rm motherCutClusters.nii
rm childCutClusters.nii
rm outputLog
