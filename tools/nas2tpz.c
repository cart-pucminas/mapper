/*
 * Copyright(C) 2011-2015 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * This file is part of Mapper.
 *
 * Mapper is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mapper is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MyLib. If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <mylib/util.h>

/**
 * @brief Mega: 10^6
 */
#define MEGA 1000000

/**
 * @brief NAS trace filename.
 */
static const char *nasfile = NULL;

/**
 * @brief NoC topology.
 */
static struct
{
	unsigned nprocs;    /**< Number of processors. */
	unsigned nrows;     /**< Number of rows.       */
	unsigned ncols;     /**< Number of columns.    */
} topology = { 0, 0, 0 };

/**
 * @brief Converts NAS trace file to Topaz input file.
 */
static void nas2tpz(FILE *nasfilep)
{
	float start;     /* Start of transmission. */
	unsigned size;   /* Size of the message.   */
	unsigned dest;   /* Destination process.   */
	unsigned source; /* Source process.        */
	float offset;    /* Time offset.           */
	
	/* Convert NAS trace file. */
	offset = -1.0;
	while (fscanf(nasfilep, "%f %u %u %u", &start, &source, &dest, &size) != EOF)
	{
		if (offset < 0.0)
			offset = start;

		start -= offset;
		start *= MEGA;
		
		fprintf(stdout, "%u %u %u %u %u %u %u %u\n",
			(unsigned)floor(start),
			source/topology.ncols, source%topology.ncols, 0,
			dest/topology.ncols, dest%topology.ncols, 0,
			size);
	}
}

/**
 * @brief Prints program usage and exits.
 */
static void usage(void)
{
	printf("Usage: nas2tpz <nas file> <nprocs> <height>x<width>\n");
	printf("Brief: converts a NAS trace file to a Topaz input file.\n");
	exit(EXIT_SUCCESS);
}

/**
 * @brief Reads command line arguments.
 */
static void readargs(int argc, char **argv)
{	
	/* Missing arguments. */
	if (argc != 4)
		usage();
		
	nasfile = argv[1];
	sscanf(argv[2], "%u", &topology.nprocs);
	sscanf(argv[3], "%u%*c%u", &topology.nrows, &topology.ncols);
	
	/* Bad topology. */
	if (topology.nrows*topology.ncols != topology.nprocs)
		error("bad topology");
}

/**
 * @brief Converts a NAS trace file to a Topaz input file.
 */
int main(int argc, char **argv)
{
	FILE *nasfilep;
	
	readargs(argc, argv);
	
	/* Open NAS trace file. */
	nasfilep = fopen(nasfile, "r");
	if (nasfilep == NULL)
		error("cannot open NAS trace file");
		
	nas2tpz(nasfilep);

	fclose(nasfilep);
	
	return (EXIT_SUCCESS);
}
