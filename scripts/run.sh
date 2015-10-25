#!/bin/bash

INDIR=examples/distributed

for kernel in CG EP FT IS MG; do
	echo $kernel
	cut -d" " -f2- $INDIR/$kernel-32.trace > input
	bin/mapper --verbose --kmeans 4 --topology 4x8 --communication input > /dev/null
	rm input
	
	cut -d" " -f2- $INDIR/$kernel-32.trace > input
	bin/mapper --verbose --kmeans 4 --topology 4x8 --hierarchical --communication input > /dev/null
	rm input
done
