#!/bin/bash

INDIR=examples
NCLUSTERS=4
MAPPER="bin/mapper --verbose"

#
# Runs kmeans strategy.
#  $1 Number of processes.
#  $2 Processor topology.
#  $3 Kernel.
#  $4 Seed value.
#
function run_kmeans
{	
	# Build command.
	topology="--topology $2"
	infile="--input $INDIR/$3/$1.trace"
	cmd="$MAPPER --seed $4 $topology $infile --kmeans $NCLUSTERS"
	
	output=$(($cmd 1> /dev/null) 2>&1)
	
	# Print only valid output.
	if [ $? == "0" ] ; then
		echo "kmeans;$3;$1;${output//[[:blank:]]/}"
	fi
}

#
# Runs hierarchical strategy.
#  $1 Number of processes.
#  $2 Processor topology.
#  $3 Kernel.
#  $4 Seed value.
#
function run_hierarchical
{	
	# Build command.
	topology="--topology $2"
	infile="--input $INDIR/$3/$1.trace"
	cmd="$MAPPER --seed $4 $topology $infile --hierarchical"
	
	output=$(($cmd 1> /dev/null) 2>&1)
	
	# Print only valid output.
	if [ $? == "0" ] ; then
		echo "hierarchical;$3;$1;${output//[[:blank:]]/}"
	fi
}

#
# Runs greedy strategy.
#  $1 Number of processes.
#  $2 Processor topology.
#  $3 Kernel.
#
function run_greedy
{	
	# Build command.
	topology="--topology $2"
	infile="--input $INDIR/$3/$1.trace"
	cmd="$MAPPER $topology $infile --greedy"
	
	output=$(($cmd 1> /dev/null) 2>&1)
	
	# Print only valid output.
	if [ $? == "0" ] ; then
		echo "greedy;$3;$1;${output//[[:blank:]]/}"
	fi
}

seeds=( 28 1013546 1013546 0 0 )
kernels=( CG EP FT IS MG )

for i in {0..4}; do
	run_kmeans        32   4x8 ${kernels[$i]} ${seeds[$i]}
	run_hierarchical  32   4x8 ${kernels[$i]} ${seeds[$i]}
	run_greedy        32   4x8 ${kernels[$i]}
	run_kmeans        64   8x8 ${kernels[$i]} ${seeds[$i]}
	run_hierarchical  64   8x8 ${kernels[$i]} ${seeds[$i]}
	run_greedy        64   8x8 ${kernels[$i]}
	run_kmeans       128  8x16 ${kernels[$i]} ${seeds[$i]}
	run_hierarchical 128  8x16 ${kernels[$i]} ${seeds[$i]}
	run_greedy       128  8x16 ${kernels[$i]}
	run_kmeans       256 16x16 ${kernels[$i]} ${seeds[$i]}
	run_hierarchical 256 16x16 ${kernels[$i]} ${seeds[$i]}
	run_greedy       256 16x16 ${kernels[$i]}
done
