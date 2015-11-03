/*
 * Copyright(C) 2015 Amanda Amorim <amandamp.amorim@gmail.com>
 *                   Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of Mapper.
 * 
 * Mapper is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Mapper is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Mapper. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>

#include <mylib/util.h>
#include <mylib/object.h>
#include <mylib/hash.h>
#include <mylib/cache.h>
#include <mylib/matrix.h>

#include "access.h"
#include "trace-parser.h"

/* Cache size (in objects). */
#define CACHE_SIZE 84769091 

/* Program arguments. */
static int ntraces = 0;          /* Number of trace files. */
static char **tracefiles = NULL; /* Trace files.           */
static char *outfile = NULL;     /* Output file.           */

/**
 * @brief Prints program usage and exits.
 */
static void usage(void)
{
	printf("usage: trace-parser <trace files> <outputfile>\n");
	exit(EXIT_SUCCESS);
}

/**
 * @brief Gets program arguments.
 */
static void readargs(int argc, char **argv)
{
	/* Bad usage. */
	if (argc < 3)
		usage();

	tracefiles = &argv[1];
	ntraces = argc - 2;
	outfile = argv[argc - 1];
}

int main(int argc, char **argv)
{
	FILE *swp;
	struct cache *c;
	struct matrix *m;	
	FILE *matrix_shared;

	readargs(argc, argv);

	if ((swp = fopen("swap", "w+")) == NULL)
		error("cannot open swap file");

	c = cache_create(&access_info, swp, CACHE_SIZE);

	/* Read traces. */
	for (int i = 0; i < ntraces; i++)
	{
		FILE *trace;
		
		if ((trace = fopen(tracefiles[i], "r")) == NULL)
			error("cannot open trace file");
		
		trace_read(c, trace, i);
		fclose(trace);
	}
	
	/* Flushe traces on swap file. */
	cache_flush(c);
	
	/* Create communication matrix. */
	fseek(swp, 0, SEEK_SET); 
	m  = matrix_create(QTD_THREADS, QTD_THREADS);
	matrix_generate(swp, m);
	
	
	if ((matrix_shared = fopen(outfile, "w")) == NULL)
		error("cannot open output file");
	
	for (int i = 0; i < ntraces; i++)
	{
		for(int j = 0; j < ntraces; j++)
			fprintf(matrix_shared, "%d;%d;%d\n", i, j, (int) matrix_get(m, i, j));	
	}

	/* House keeping. */
	fclose(matrix_shared);
	fclose(swp);
		
	return (EXIT_SUCCESS);
}

	

