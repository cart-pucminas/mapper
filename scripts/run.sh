#!/bin/bash

INDIR=examples
OUTDIR=output
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
	cut -d" " -f2- $INDIR/$3/$1.trace > input
	
	# Build command.
	topology="--topology $2"
	infile="--input input"
	cmd="$MAPPER --seed $4 $topology $infile --kmeans $NCLUSTERS"
	
	output=$(($cmd 1> $OUTDIR/kmeans-$1-$3.map) 2>&1)
	
	# Print only valid output.
	if [ $? == "0" ] ; then
		echo "kmeans;$3;$1;${output//[[:blank:]]/}"
	fi
	
	rm -f input
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
	cut -d" " -f2- $INDIR/$3/$1.trace > input
	
	# Build command.
	topology="--topology $2"
	infile="--input input"
	cmd="$MAPPER --seed $4 $topology $infile --hierarchical"
	
	output=$(($cmd 1> $OUTDIR/kmeans-$1-$3.map) 2>&1)
	
	# Print only valid output.
	if [ $? == "0" ] ; then
		echo "hierarchical;$3;$1;${output//[[:blank:]]/}"
	fi
	
	rm -f input
}

#
# Runs greedy strategy.
#  $1 Number of processes.
#  $2 Processor topology.
#  $3 Kernel.
#
function run_greedy
{
	cut -d" " -f2- $INDIR/$3/$1.trace > input
	
	# Build command.
	topology="--topology $2"
	infile="--input input"
	cmd="$MAPPER $topology $infile --greedy"
	
	output=$(($cmd 1> $OUTDIR/kmeans-$1-$3.map) 2>&1)
	
	# Print only valid output.
	if [ $? == "0" ] ; then
		echo "greedy;$3;$1;${output//[[:blank:]]/}"
	fi
	
	rm -f input
}

seeds=( 28 1013546 1013546 0 0 )
kernels=( CG EP FT IS MG )

mkdir -p $OUTDIR
rm -rf $OUTDIR/*

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
