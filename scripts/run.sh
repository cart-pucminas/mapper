#!/bin/bash

INDIR=examples
NCLUSTERS=4
MAPPER="bin/mapper --verbose"

found="yes"

#
# Run kmeans strategy.
#  $1 Number of processes.
#  $2 Processor topology.
#  $3 Kernel.
#  $4 Hierarchical kmeans?
#  $5 Seed value.
#
function run_kmeans
{	
	# Build command.
	topology="--topology $2"
	infile="--input $INDIR/$3/$1.trace"
	if [ $4 == "y" ]; then
		cmd="$MAPPER --seed $5 $topology $infile --hierarchical"
	else
		cmd="$MAPPER --seed $5 $topology $infile --nclusters $NCLUSTERS"
	fi
	
	output=$(($cmd 1> /dev/null) 2>&1)
	
	# Print only valid output.
	if [ $? == "0" ] ; then
		echo "$4;$3;$1;${output//[[:blank:]]/}" > /dev/null
	else
		found="no"
	fi
}

seeds=( 28 X X 0 0 )
kernels=( CG EP FT IS MG )

for i in 1 2; do
	for j in {0..1000000}; do
		found="yes"
		run_kmeans 32    4x8 ${kernels[$i]} n $j
		if [ $found == "no" ]; then continue; fi
		run_kmeans 32    4x8 ${kernels[$i]} y $j
		if [ $found == "no" ]; then continue; fi
		run_kmeans 64    8x8 ${kernels[$i]} n $j
		if [ $found == "no" ]; then continue; fi
		run_kmeans 64    8x8 ${kernels[$i]} y $j
		if [ $found == "no" ]; then continue; fi
		run_kmeans 128  8x16 ${kernels[$i]} n $j
		if [ $found == "no" ]; then continue; fi
		run_kmeans 128  8x16 ${kernels[$i]} y $j
		if [ $found == "no" ]; then continue; fi
		run_kmeans 256 16x16 ${kernels[$i]} n $j
		if [ $found == "no" ]; then continue; fi
		run_kmeans 256 16x16 ${kernels[$i]} y $j
		if [ $found == "no" ]; then continue; fi
		echo found $j for ${kernels[$i]}
		break
	done
done
