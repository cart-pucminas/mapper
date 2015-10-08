/*
 * Copyright(C) 2015 Pedro H. Penna <pedrohenriquepenna@gmail.com>
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

#include <mylib/matrix.h>
#include <mylib/util.h>
#include <mylib/vector.h>

#include "mapper.h"

/* Program arguments. */
static char *filename;  /* Input filename.      */
static unsigned nprocs; /* Number of processes. */

/**
 * @brief Prints program usage and exits.
 */
static void usage(void)
{
	printf("Usage: mapper <input> <topology> <nclusters> <mindistance>\n");
	printf("Brief: maps processes on a manycore processor");
	exit(EXIT_SUCCESS);
}

/**
 * @brief Reads command line arguments.
 */
static void readargs(int argc, char **argv)
{
	filename = argv[1];
}

/**
 * @brief Reads input data.
 */
static matrix_t read_data(FILE *input)
{
	matrix_t m;         /* Communication matrix.        */
	unsigned size;      /* Size of communication.      */
	unsigned src, dest; /* Source and target processes. */
	
	m = matrix_create(nprocs, nprocs);
	
	/* Read communication matrix. */
	fseek(input, 0, SEEK_SET);
	fscanf(input, "%u %u %u\n", &src, &dest, &size);
	while (!feof(input))
	{
		matrix_set(m, dest, src, matrix_get(m, dest, src) + size);
		matrix_set(m, src, dest, matrix_get(m, src, dest) + size);
        	
		fscanf(input, "%u %u %u\n", &src, &dest, &size);
	}
	
	return (m);
}

/*
 * Mapps processes in a NoC
 */
int main(int argc, char **argv)
{
	FILE *input;
	
	/* Open input file. */
	if ((input = fopen(filename, "r")) == NULL)
		error("cannot open input file");
	
	read_data(input);
	
	/* House keeping. */
	fclose(input);
	

	return (0);
}
