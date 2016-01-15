#!/bin/bash

for ((a=0; a<10; a++)) 
do
	echo current for loop iteration is $a
	c=0;
	while [ $c -le 3 ] 
	do
		echo while iteration $c
		c=$(($c+1))	
		if [ $c -eq 3 ]
		then
			break 2;
		fi
	done
done
