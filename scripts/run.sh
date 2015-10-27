#!/bin/bash

INDIR=examples
NCLUSTERS=4
MAPPER="bin/mapper --verbose"

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
		echo "$4;$3;$1;${output//[[:blank:]]/}"
	fi
}

seeds=( 28 1013546 1013546 0 0 )
kernels=( CG EP FT IS MG )

for i in {0..4}; do
	run_kmeans 32    4x8 ${kernels[$i]} n ${seeds[$i]}
	run_kmeans 32    4x8 ${kernels[$i]} y ${seeds[$i]}
	run_kmeans 64    8x8 ${kernels[$i]} n ${seeds[$i]}
	run_kmeans 64    8x8 ${kernels[$i]} y ${seeds[$i]}
	run_kmeans 128  8x16 ${kernels[$i]} n ${seeds[$i]}
	run_kmeans 128  8x16 ${kernels[$i]} y ${seeds[$i]}
	run_kmeans 256 16x16 ${kernels[$i]} n ${seeds[$i]}
	run_kmeans 256 16x16 ${kernels[$i]} y ${seeds[$i]}
done
