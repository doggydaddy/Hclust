#!/bin/bash

file=`ls cluster*.nii`
for j in $file
do
	3dresample -dxyz 1 1 1 -rmode Cu -prefix 1mm$j -inset $j
done

file2=`ls 1mmcluster*.nii`
c=1
for i in $file2
do
	if [ "$c" -eq 1 ]; then
		first_file=$i
		c=$(($c+1))
	elif [ "$c" -eq 2 ]; then
		3dcalc -a $first_file -b $i -expr "a+($c*b)" -prefix merged$c.nii
		c=$(($c+1))
	else
		3dcalc -a merged$(($c-1)).nii -b $i -expr "a+($c*b)" -prefix merged$c.nii
		c=$(($c+1))
	fi
done

