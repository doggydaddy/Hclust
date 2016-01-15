#!/bin/bash

# need manual in folder:
# cc matrix of previous hierarchy
# mask of previous heriarchy
# mask of current hierarchy

#cp ../../hclust_avg/*.R .
#cp ../../hclust_avg/*.sh .
#cp ../../hclust_avg/cutTree .
#cp ../../hclust_avg/hClust .
#cp ../../hclust_avg/extractClusterCC .
#cp ../../hclust_avg/makeDist .

cp ~/Documents/resources/TT1mm.nii .
#cp ~/Documents/resources/threshold_plot.R .


# PARAMS
# 1. CC MATRIX OF PREV. HIER.
# 2. MASK OF PREF. HIER
# 3. MASK OF CUR. HIER
# 4. TO CUT LEVEL


#Hierarchical clustering
extractClusterCC $2 $3 $1 cur_hier.cc
makeDist cur_hier.cc cur_hier.dist 1
hClust cur_hier.dist $3 tmp.nii 2

rm tmp.nii
mv saved_tree cur_hier.tree

#Analysis
outputResults.sh cur_hier.tree $3 $4 cut_4d.nii diff_4d.nii >/dev/null 2>&1
analyze_createdChildrenAt.sh $3 cur_hier.cc > /dev/null 2>&1
filterTree.R cutStats.txt threshold_plot.R cutStatsF.txt >/dev/null 2>&1
buildTree.R cutStatsF.txt tableF.txt >/dev/null 2>&1
makeDendrogram.R cur_hier.tree tableF.txt dendrogram.rda >/dev/null 2>&1
makeLeaves.R tableF.txt >/dev/null 2>&1
mergeClusters.sh



