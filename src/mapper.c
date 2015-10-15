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

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include <mylib/matrix.h>
#include <mylib/vector.h>
#include <mylib/util.h>

#include "mapper.h"

/* Forward definitions. */
extern int *map_kmeans(const vector_t *, int, void *);

/**
 * @brief Number of mapping strategies.
 */
#define NR_STRATEGIES 1

/**
 * @brief Mapping strategy.
 */
typedef int *(*strategy)(const vector_t *, int, void *);

/**
 * @brief Mapping strategies.
 */
static strategy strategies[NR_STRATEGIES] =  {
	map_kmeans
};

/**
 * @brief Maps process.
 */
int *process_map(matrix_t communication, int strategy, void *args)
{
	int *map;        /* Map.                 */
	int nprocs;      /* Number of processes. */
	vector_t *procs; /* Processes.           */
	
	/* Sanity check. */
	assert(communication != NULL);
	assert(matrix_height(communication) == matrix_width(communication));
	assert(strategy < NR_STRATEGIES);
	assert(args != NULL);
	
	nprocs = matrix_height(communication);
	
	/* Create processes. */
	procs = smalloc(nprocs*sizeof(vector_t));
	for (int i = 0; i < nprocs; i++)
	{
		procs[i] = vector_create(nprocs);
		for (int j = 0; j < nprocs; j++)
		{
			double a;
			a = matrix_get(communication, i, j);
			vector_set(procs[i], j, (a > 0) ? 1.0/a : a);
		}
	}

	map = strategies[strategy](procs, nprocs, args);

	/* House keeping. */
	for (int i = 0; i < nprocs; i++)
		vector_destroy(procs[i]);
	free(procs);
	
	return (map);
}
