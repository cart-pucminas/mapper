#!/bin/bash

# Copyright(C) 2015 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#
# This file is part of Mapper.
#
# Mapper is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Mapper is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MyLib. If not, see <http://www.gnu.org/licenses/>.

# Directories.
BINDIR=bin
INDIR=examples
OUTDIR=output

# Tools.
MAP2NAS="$BINDIR/map2nas"
MAPPER="$BINDIR/mapper --verbose"
NAS2TPZ="$BINDIR/nas2tpz"

# Script parameters.
INSTRUMENT=$1 # Instrument NAS trace file? 
NCLUSTERS=$2  # Number of clusters.

#
# Runs kmeans strategy.
#  $1 Number of processes.
#  $2 Processor topology.
#  $3 Kernel.
#  $4 Seed value.
#
function run_kmeans
{
	tracefile="$INDIR/$3/$1.trace"
	mapfile="$OUTDIR/kmeans-$1-$3.map"
	nasfile="$OUTDIR/kmeans-$1-$3.trace"
	tpzfile="$OUTDIR/greedy-$1-$3.tpz.trace"
	
	cut -d" " -f2- $tracefile > input
	
	# Build command.
	topology="--topology $2"
	infile="--input input"
	cmd="$MAPPER --seed $4 $topology $infile --kmeans $NCLUSTERS"
	
	output=$(($cmd 1> $mapfile) 2>&1)
	
	# Print only valid output.
	if [ $? == "0" ] ; then
		echo "kmeans;$3;$1;${output//[[:blank:]]/}"
	fi

	# Instrument NAS file.
	if [ $INSTRUMENT == "yes" ]; then
		$MAP2NAS $tracefile $mapfile > $nasfile
		$NAS2TPZ $nasfile $1 $2 > $tpzfile
	fi
	
	# House keeping.
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
	tracefile="$INDIR/$3/$1.trace"
	mapfile="$OUTDIR/hierarchical-$1-$3.map"
	nasfile="$OUTDIR/hierarchical-$1-$3.trace"
	tpzfile="$OUTDIR/greedy-$1-$3.tpz.trace"
	
	cut -d" " -f2- $tracefile > input
	
	# Build command.
	topology="--topology $2"
	infile="--input input"
	cmd="$MAPPER --seed $4 $topology $infile --hierarchical"
	
	output=$(($cmd 1> $mapfile) 2>&1)
	
	# Print only valid output.
	if [ $? == "0" ] ; then
		echo "hierarchical;$3;$1;${output//[[:blank:]]/}"
	fi
	
	# Instrument NAS file.
	if [ $INSTRUMENT == "yes" ]; then
		$MAP2NAS $tracefile $mapfile > $nasfile
		$NAS2TPZ $nasfile $1 $2 > $tpzfile
	fi
	
	# House keeping.
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
	tracefile="$INDIR/$3/$1.trace"
	mapfile="$OUTDIR/greedy-$1-$3.map"
	nasfile="$OUTDIR/greedy-$1-$3.trace"
	tpzfile="$OUTDIR/greedy-$1-$3.tpz.trace"
	
	cut -d" " -f2- $tracefile > input
	
	# Build command.
	topology="--topology $2"
	infile="--input input"
	cmd="$MAPPER $topology $infile --greedy"
	
	output=$(($cmd 1> $mapfile) 2>&1)
	
	# Print only valid output.
	if [ $? == "0" ] ; then
		echo "greedy;$3;$1;${output//[[:blank:]]/}"
	fi
	
	# Instrument NAS file.
	if [ $INSTRUMENT == "yes" ]; then
		$MAP2NAS $tracefile $mapfile > $nasfile
		$NAS2TPZ $nasfile $1 $2 > $tpzfile
	fi
	
	# House keeping.
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
