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

#include <stdarg.h>

#include <mylib/list.h>
#include <mylib/matrix.h>
#include <mylib/vector.h>
#include <mylib/ai.h>
#include <mylib/util.h>

#include "mapper.h"

/*===========================================================================*
 *                                Kmeans Strategy                            *
 *===========================================================================*/

/**
 * @brief Computes centroids.
 *
 * @param data      Target data.
 * @param map       Cluster map.
 * @param nclusters Number of clusters.
 *
 * @returns Centroids.
 */
static vector_t *compute_centroids
(const vector_t *data, int npoints, int *map, unsigned nclusters)
{
	int *count;          /* Number points in each cluster. */
	vector_t *centroids; /* Centroids.                     */

	/* Create centroids. */
	centroids = smalloc(nclusters*sizeof(vector_t));
	count = smalloc(nclusters*sizeof(int));
	for (unsigned i = 0; i < nclusters; i++)
	{
		count[i] = 0;
		centroids[i] = vector_create(vector_dimension(data[i]));
	}

	/* Compute centroids. */
	for (int i = 0; i < npoints; i++)
	{
		count[map[i]]++;
		vector_add(centroids[map[i]], data[i]);
	}
	for (unsigned i = 0; i < nclusters; i++)
		vector_scalar(centroids[i], count[i]);

	/* House keeping. */
	free(count);

	return (centroids);
}


/**
 * @brief Maps processes using kmeans algorithm.
 *
 * @param procs     Processes.
 * @param nprocs    Number of processes.
 * @param nclusters Number of clusters.
 *
 * @returns A process map.
 */
static int *map_kmeans(const vector_t *procs, unsigned nprocs, unsigned nclusters)
{
	int *map;            /* Process map. */
	vector_t *centroids; /* Centroids.   */

	map = kmeans(procs, nprocs, nclusters, 0.0);

	centroids = compute_centroids(procs, nprocs, map, nclusters);

	/* House keeping. */
	for (unsigned i = 0; i < nclusters; i++)
		vector_destroy(centroids[i]);
	free(centroids);

	return (map);
}

/*===========================================================================*
 *                               Public Interface                            *
 *===========================================================================*/

/**
 * @brief Maps process.
 */
int *process_map(unsigned nprocs, matrix_t communication, unsigned nclusters)
{
	int *map;        /* Map.       */
	vector_t *procs; /* Processes. */
	
	/* Create processes. */
	procs = smalloc(nprocs*sizeof(vector_t));
	for (unsigned i = 0; i < nprocs; i++)
	{
		procs[i] = vector_create(nprocs);
		
		for (unsigned j = 0; j < nprocs; j++)
			vector_set(procs[i], j, matrix_get(communication, i, j));
	}

	map = map_kmeans(procs, nprocs, nclusters);

	/* House keeping. */
	for (unsigned i = 0; i < nprocs; i++)
		vector_destroy(procs[i]);
	free(procs);
	
	return (map);
}
