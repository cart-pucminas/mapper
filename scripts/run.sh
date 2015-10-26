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
#
function run_kmeans
{	
	# Build command.
	topology="--topology $2"
	infile="--input $INDIR/$3/$1.trace"
	if [ $4 == "y" ]; then
		cmd="$MAPPER $topology $infile --hierarchical"
	else
		cmd="$MAPPER $topology $infile --nclusters $NCLUSTERS"
	fi
	
	output=$(($cmd 1> /dev/null) 2>&1)
	
	# Print only valid output.
	if [ $? == "0" ] ; then
		echo "$4;$3;$1;${output//[[:blank:]]/}"
	fi
}

for kernel in CG EP FT IS MG; do
	run_kmeans 32    4x8 $kernel n
	run_kmeans 32    4x8 $kernel y
	run_kmeans 64    8x8 $kernel n
	run_kmeans 64    8x8 $kernel y
	run_kmeans 128  8x16 $kernel n
	run_kmeans 128  8x16 $kernel y
	run_kmeans 256 16x16 $kernel n
	run_kmeans 256 16x16 $kernel y
done
