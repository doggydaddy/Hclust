#!/bin/bash
# For two given createdChildrenAt* datasets, find if the first one is the children of one of clusters in the second. Output 0 if not. If yes, output 1 or 2 depending on the which one of the clusters in the second dataset that is the mother.
3dcalc -a $1 -expr "ispositive(a)" -prefix tmp_children.nii
3dcalc -a $2 -expr "within(a,1,1)" -prefix tmp_m1.nii
3dcalc -a $2 -expr "within(a,2,2)" -prefix tmp_m2.nii
3dcalc -a tmp_children.nii -b tmp_m1.nii -expr "and(a,b)" -prefix test_m1.nii
3dcalc -a tmp_children.nii -b tmp_m2.nii -expr "and(a,b)" -prefix test_m2.nii
3dmaskave test_m1.nii > res
result=`less res`
set -- $result
if [ "$1" != "0" ]
then
	echo "1"
	rm res
	rm test_m1.nii
	rm tmp_m1.nii
	rm test_m2.nii
	rm tmp_m2.nii
	rm tmp_children.nii
	exit 0;
fi
rm res
3dmaskave test_m2.nii > res
result=`less res`
set -- $result
if [ "$1" != "0" ]
then
	echo "2"
else
	echo "0"
fi
rm res

rm test_m1.nii
rm tmp_m1.nii
rm test_m2.nii
rm tmp_m2.nii
rm tmp_children.nii

