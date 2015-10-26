#!/bin/bash

INDIR=examples
NCLUSTERS=4

function run_kmeans {
	bin/mapper --verbose --topology $2 --communication $INDIR/$3/$1.trace \
	--nclusters $NCLUSTERS  > /dev/null
}

function run_hierarchical_kmeans {
	bin/mapper --verbose --topology $2 --communication $INDIR/$3/$1.trace \
	--hierarchical  > /dev/null
}

for kernel in CG EP FT IS MG; do		
	echo $kernel-32	
		run_kmeans 32 4x8  $kernel
#		run_hierarchical_kmeans 32 4x8  $kernel
	echo $kernel-64
		run_kmeans 64 8x8  $kernel
#		run_hierarchical_kmeans 64 8x8  $kernel
	echo $kernel-128
		run_kmeans 128 8x16 $kernel
#		run_hierarchical_kmeans 128 8x16 $kernel
	echo $kernel-256
		run_kmeans 256 16x16 $kernel
#		run_hierarchical_kmeans 256 16x16 $kernel
done
