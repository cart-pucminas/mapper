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
#include <string.h>

#include <mylib/matrix.h>
#include <mylib/util.h>
#include <mylib/vector.h>

#include "mapper.h"

#define USE_KMEANS        (1 << 0)
#define USE_COMMUNICATION (1 << 1)
#define USE_TOPOLOGY      (1 << 2)
#define USE_AUCTION       (1 << 3)

/* Program arguments. */
static unsigned flags = 0;         /* Argument flags.       */
static FILE *communication = NULL; /* Communication matrix. */
static FILE *topology = NULL;      /* Topology filename.    */
static unsigned nprocs = 0;        /* Number of processes.  */
static unsigned nclusters = 0;     /* Number of clusters.   */

/**
 * @brief Prints program usage and exits.
 */
static void usage(void)
{
	printf("Usage: mapper [options] --nprocs <num> --communication <filename>\n\n");
	printf("Brief maps processes on a processor\n\n");
	printf("Options:\n");
	printf("    --auction-balance     use auction balance");
	printf("    --kmeans <nclusters>  use kmeans strategy\n");
	printf("    --topology <filename> topology file\n");
	
	exit(EXIT_SUCCESS);
}

/**
 * @brief Reads command line arguments.
 */
static void readargs(int argc, char **argv)
{
	/* Parsing states. */
	enum parsing_states {
		STATE_READ_ARG,         /* Read argument.           */
		STATE_SET_AUCTION,      /* Set auction balance.     */
		STATE_SET_KMEANS,       /* Set kmeans parameters.   */
		STATE_SET_TOPOLOGY,     /* Set topology file.       */
		STATE_SET_NPROCS,       /* Set number of processes. */
		STATE_SET_COMMUNICATION /* Set communication file.  */
	};
	
	int state;
	
	/* Parse command line parameters. */
	state = STATE_READ_ARG;
	for (int i = 1; i < argc; i++)
	{
		char *arg = argv[i];
		
		/* Set value. */
		if (state != STATE_READ_ARG)
		{
			switch (state)
			{
				/* Set auction balance. */
				case STATE_SET_AUCTION:
					flags |= USE_AUCTION;
					break;
					
				/* Set kmeans parameters. */
				case STATE_SET_KMEANS:
					flags |= USE_KMEANS;
					nclusters = atoi(arg);
					break;
					
				/* Set topology file. */
				case STATE_SET_TOPOLOGY:
					flags |= USE_TOPOLOGY;
					if (topology != NULL)
						topology = freopen(arg, "r", topology);
					else
						topology = fopen(arg, "r");
					break;
				
				/* Set number of processes. */
				case STATE_SET_NPROCS:
					nprocs = atoi(arg);
					break;
				
				/* Set communication files. */
				case STATE_SET_COMMUNICATION:
					flags |= USE_COMMUNICATION;
					if (communication != NULL)
						communication = freopen(arg, "r", communication);
					else
						communication = fopen(arg, "r");
					break;
				
				/* Wrong usage. */
				default:
					usage();
			}
			
			state = STATE_READ_ARG;
			continue;
		}
		
		/* Parse argument. */
		if (!strcmp(arg, "--help"))
			usage();
		else if (!strcmp(arg, "--auction-balance"))
			state = STATE_SET_AUCTION;
		else if (!strcmp(arg, "--kmeans"))
			state = STATE_SET_KMEANS;
		else if(!strcmp(arg, "--topology"))
			state = STATE_SET_TOPOLOGY;
		else if (!strcmp(arg, "--nprocs"))
			state = STATE_SET_NPROCS;
		else if (!strcmp(arg, "--communication"))
			state = STATE_SET_COMMUNICATION;
	}
}

/**
 * @brief Checks program arguments.
 */
static void chkargs(void)
{
	if (communication == NULL)
		error("cannot open communication file");
	if ((flags & USE_TOPOLOGY) && (topology == NULL))
		error("cannot open topology file");
	if (nprocs == 0)
		error("invalid number of processes");
	if (nclusters == 0)
		error("invalid kmeans parameters");
}

/**
 * @brief Reads communication matrix.
 * 
 * @param file Target file.
 * 
 * @returns Communication matrix.
 */
static matrix_t read_communication_matrix(FILE *input)
{
	matrix_t m;         /* Communication matrix.        */
	unsigned size;      /* Size of communication.       */
	unsigned src, dest; /* Source and target processes. */
	
	m = matrix_create(nprocs, nprocs);
	
	/* Read communication matrix. */
	fseek(input, 0, SEEK_SET);
	while (fscanf(input, "%u %u %u\n", &src, &dest, &size) != EOF)
	{
		matrix_set(m, dest, src, matrix_get(m, dest, src) + size);
		matrix_set(m, src, dest, matrix_get(m, src, dest) + size);
	}
	
	return (m);
}

/*
 * Mapps processes in a NoC
 */
int main(int argc, char **argv)
{
	int *map;
	matrix_t m;
	
	readargs(argc, argv);
	chkargs();
	
	m = read_communication_matrix(communication);
	
	map = process_map(m, STRATEGY_KMEANS, nclusters, flags & USE_AUCTION);
	
	/* Print map. */
	for (unsigned i = 0; i < nprocs; i++)
		printf("%3u %d\n", i, map[i]);
	
	/* House keeping. */
	free(map);
	matrix_destroy(m);
	fclose(communication);
	if (flags & USE_TOPOLOGY)
		fclose(topology);
	
	return (0);
}
