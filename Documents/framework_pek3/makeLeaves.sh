#!/bin/bash

# read first arguments (table) line by line
while IFS=, read -r line; do
	IFS=, read -r x y z <<-EOI
	$line
	EOI
	xx=$x
	yy=$y
	zz=$z
	echo "$xx $yy $zz"
	if (( "$yy" < 0 )); then
		yy_abs=${$yy#-}
		if (( "$xx" < "10" )); then
			3dcalc -a createdChildrenAt00$xx.nii -expr "$yy_abs*within(a,1,1)" -prefix cluster00$xx.nii
		else
			3dcalc -a createdChildrenAt0$xx.nii -expr "$yy_abs*within(a,1,1)" -prefix cluster0$xx.nii
		fi
		zz_abs=${$zz#-}
		if (( "$xx" < "10" )); then
			3dcalc -a createdChildrenAt00$xx.nii -expr "$zz_abs*within(a,2,2)" -prefix cluster00$xx.nii
		else
			3dcalc -a createdChildrenAt0$xx.nii -expr "$zz_abs*within(a,2,2)" -prefix cluster0$xx.nii
		fi
	fi
done < "$1"
