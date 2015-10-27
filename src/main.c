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
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <mylib/matrix.h>
#include <mylib/util.h>
#include <mylib/vector.h>

#include "mapper.h"

/**
 * @brief Program flags.
 */
/**@{*/
#define USE_COMMUNICATION (1 << 0)
#define USE_HIERARCHICAL  (1 << 1)
/**@}*/

/* Program arguments. */
static unsigned flags = 0;                            /* Argument flags.       */
static FILE *input = NULL;                            /* Input file.           */
static int nclusters = 0;                             /* Number of clusters.   */
static struct processor proc = {0, 0, 0, NULL, NULL}; /* Processor's topology. */
bool verbose = false;                                 /* Be verbose.           */
unsigned seed = 0;                                    /* Seed for randomness.  */

/**
 * @brief Number of processes.
 */
static int nprocs = 0;

/**
 * @brief Prints program usage and exits.
 */
static void usage(void)
{
	printf("Usage: mapper [options] --topology <height>x<width> --input <filename>\n\n");
	printf("Brief maps processes on a processor\n\n");
	printf("Options:\n");
	printf("    --help                  display this information\n");
	printf("    --hierarchical          use hierarchical mapping\n");
	printf("    --nclusters <nclusters> set number of clusters in kmeans\n");
	printf("    --seed <value>          set sed value\n");
	printf("    --verbose               be verbose\n");
	
	exit(EXIT_SUCCESS);
}

/**
 * @brief Reads command line arguments.
 */
static void readargs(int argc, char **argv)
{
	/* Parsing states. */
	enum parsing_states {
		STATE_READ_ARG,      /* Read argument.         */
		STATE_SET_NCLUSTERS, /* Set kmeans parameters. */
		STATE_SET_TOPOLOGY,  /* Set topology file.     */
		STATE_SET_INPUT,     /* Set input file.        */
		STATE_SET_SEED       /* Set seed value.        */
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
				/* Set kmeans parameters. */
				case STATE_SET_NCLUSTERS:
					nclusters = atoi(arg);
					break;
					
				/* Set topology file. */
				case STATE_SET_TOPOLOGY:
					sscanf(arg, "%u%*c%u", &proc.height, &proc.width);
					break;
				
				/* Set input file. */
				case STATE_SET_INPUT:
					if (input != NULL)
						input = freopen(arg, "r", input);
					else
						input = fopen(arg, "r");
					break;
				
				/* Set seed value. */
				case STATE_SET_SEED:
					sscanf(arg, "%u", &seed);
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
		else if (!strcmp(arg, "--hierarchical"))
			flags |= USE_HIERARCHICAL;
		else if (!strcmp(arg, "--nclusters"))
			state = STATE_SET_NCLUSTERS;
		else if(!strcmp(arg, "--topology"))
			state = STATE_SET_TOPOLOGY;
		else if (!strcmp(arg, "--input"))
			state = STATE_SET_INPUT;
		else if (!strcmp(arg, "--seed"))
			state = STATE_SET_SEED;
		else if (!strcmp(arg, "--verbose"))
			verbose = true;
	}
}

/**
 * @brief Checks program arguments.
 */
static void chkargs(void)
{
	if (input == NULL)
		error("cannot open input file");
	if ((proc.height == 0) || (proc.width == 0))
		error("bad processor's dimensions");
	if (!(flags & USE_HIERARCHICAL) && (nclusters == 0))
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
	matrix_t m;    /* Communication matrix.        */
	int size;      /* Size of communication.       */
	int src, dest; /* Source and target processes. */
	
	m = matrix_create(nprocs, nprocs);
	
	/* Read communication matrix. */
	fseek(input, 0, SEEK_SET);
	while (fscanf(input, "%d %d %d\n", &src, &dest, &size) != EOF)
	{
		matrix_set(m, dest, src, matrix_get(m, dest, src) + size);
		matrix_set(m, src, dest, matrix_get(m, src, dest) + size);
	}
	
	return (m);
}

/**
 * @brief Returns the core ID of a processor.
 * 
 * @param i Vertical location.
 * @param j Horizontal location.
 * 
 * @returns The core ID of a processor.
 */
static inline int processor_coreid(int i, int j)
{
	return (i*proc.width + j);
}

/**
 * @brief Setups processor.
 */
static void processor_setup(void)
{
	proc.ncores = proc.height*proc.width;
	
	/* Allocate topology. */
	proc.topology = smalloc(proc.ncores*sizeof(int *));
	for (int i = 0; i < proc.ncores; i++)
		proc.topology[i] = scalloc(proc.ncores, sizeof(int));
	
	/* Allocate nlinks. */
	proc.nlinks = scalloc(proc.ncores, sizeof(int));
	
	/* Setup. */
	for (int i = 0; i < proc.height; i++)
	{
		for (int j = 0; j < proc.width; j++)
		{
			int id;
			
			id = processor_coreid(i, j);
			
			if ((i - 1) >= 0)
				proc.topology[id][processor_coreid(i-1,j)]=1, proc.nlinks[id]++;
			if ((i + 1) < proc.height)
				proc.topology[id][processor_coreid(i+1,j)]=1, proc.nlinks[id]++;
			if ((j - 1) >= 0)
				proc.topology[id][processor_coreid(i,j-1)]=1, proc.nlinks[id]++;
			if ((j + 1) < proc.width)
				proc.topology[id][processor_coreid(i,j+1)]=1, proc.nlinks[id]++;
		}
	}
}

/**
 * @brief Destroys processor.
 */
static void processor_destroy(void)
{
	for (int i = 0; i < proc.height; i++)
		free(proc.topology[i]);
	free(proc.topology);
	free(proc.nlinks);
}

/**
 * @brief Evaluates how good a process map is.
 * 
 * @param map     Process map.
 * @param nprocs  Number of processes.
 * @param traffic Communication matrix.
 * 
 * @returns Process map fitness.
 */
static double evaluate(int *map, int nprocs, matrix_t traffic)
{
	double fitness;
	
	/* Sanity check. */
	assert(map != NULL);
	assert(nprocs > 0);
	assert(traffic != NULL);
	
	/* Evaluate map. */
	fitness = 0.0;
	for (int i = 0; i < nprocs; i++)
	{
		for (int j = 0; j < nprocs; j++)
		{
			int distance;
			
			/* Skip this process. */
			if (j == i)
				continue;
		
			distance = (map[i]/proc.width > map[j]/proc.width) ?
						map[i]/proc.width - map[j]/proc.width :
						map[j]/proc.width - map[i]/proc.width +
					   (map[i]%proc.width > map[j]%proc.width) ?
					    map[i]%proc.width - map[j]%proc.width :
					    map[j]%proc.width - map[i]%proc.width;
						
			fitness += distance*matrix_get(traffic, i, j);
		}
	}
	
	return (fitness/(nprocs*nprocs));
}

/*
 * Maps processes in a NoC
 */
int main(int argc, char **argv)
{
	int *map;
	matrix_t m;
	struct kmeans_args args;
	
	readargs(argc, argv);
	chkargs();
	
	processor_setup();
	
	nprocs = proc.height*proc.width;
	
	m = read_communication_matrix(input);
	
	srandnum(seed);
	
	/* Build strategy arguments. */
	args.nclusters = nclusters;
	args.proc = &proc;
	args.hierarchical = (flags & USE_HIERARCHICAL) ? 1 : 0;
	
	map = process_map(m, STRATEGY_KMEANS, &args);
	
	/* Print map. */
	for (int i = 0; i < nprocs; i++)
		printf("%3u %d\n", i, map[i]);
	if (verbose)
		fprintf(stderr, " %lf\n", evaluate(map, nprocs, m));
	
	/* House keeping. */
	free(map);
	matrix_destroy(m);
	processor_destroy();
	fclose(input);
	
	return (0);
}
