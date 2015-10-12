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

#include <mylib/algorithms.h>
#include <mylib/list.h>
#include <mylib/matrix.h>
#include <mylib/vector.h>
#include <mylib/ai.h>
#include <mylib/util.h>

#include "mapper.h"

/*========================================================================*
 *                             Kmeans Strategy                            *
 *========================================================================*/

/**
 * @brief Computes the average distance inside clusters.
 * 
 * @param data      Target data.
 * @param npoints   Number of data points.
 * @param centroids Centroids of clusters.
 * @param nclusters Number of clusters.
 * @param map       Cluster map.
 *
 * @returns Average distance in clusters.
 */
static double *compute_average_distance
(const vector_t *data, int npoints, const vector_t *centroids, int nclusters, int *map)
{
	int *count;  /* Count for average. */
	double *avg; /* Average distance.   */

	count = smalloc(nclusters*sizeof(int));
	avg = smalloc(nclusters*sizeof(double));

	/* Compute average distance in clusters. */
	for (int i = 0; i < nclusters; i++)
	{
		count[i] = 0;
		avg[i] = 0.0;

		for (int j = 0; j < npoints; j++)
		{
			/* Not in this cluster. */
			if (map[j] != i)
				continue;

			count[i]++;
			avg[i] += vector_distance(centroids[i], data[j]);
		}
		if (count[i] > 0)
			avg[i] /= count[i];
	}

	/* House keeping. */
	free(count);

	return (avg);
}

/**
 * @brief Computes centroids.
 *
 * @param data      Target data.
 * @param npoints   Number of data points.
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
	{
		if (count[i] > 0)
			vector_scalar(centroids[i], count[i]);
	}

	/* House keeping. */
	free(count);

	return (centroids);
}

/**
 * @brief Destroys centroids.
 * 
 * @param centroids Centroids.
 * @param ncentroids Number of centroids.
 */
static void destroy_centroids(vector_t *centroids, unsigned ncentroids)
{
	for (unsigned i = 0; i < ncentroids; i++)
		vector_destroy(centroids[i]);
}

/**
 * @brief Balances processes among clusters using auction's algorithm.
 * 
 * @param procs     Processes.
 * @param nprocs    Number of processes.
 * @param centroids Centroids of clusters.
 * @param nclusters Number of clusters.
 */
static int *auction_balance
(const vector_t *procs, unsigned nprocs, const vector_t *centroids, unsigned nclusters)
{
	int *map;   /* Process map.      */
	matrix_t m; /* Auction's matrix. */
	unsigned n; /* nprocs/nclusters  */
	
	if (nprocs%nclusters)
		error("invalid number of clusters");
	
	n = nprocs/nclusters;
	m = matrix_create(nprocs, nprocs);
	
	/* Build auction's matrix. */
	for (unsigned i = 0; i < nprocs; i++)
	{
		for (unsigned j = 0; j < nclusters; j++)
		{
			double distance;
			
			distance = vector_distance(procs[i], centroids[j]);
			for (unsigned k = 0; k < n; k++)
				matrix_set(m, i, n*j + k, distance);
		}
	}
	
	map = auction(m, 0.0001);
	
	/* Fix map. */
	for (unsigned i = 0; i < nprocs; i++)
		map[i] /= n;
	
	/* House keeping. */
	matrix_destroy(m);
	
	return (map);
}

/**
 * @brief Maps processes using kmeans algorithm.
 *
 * @param procs       Processes.
 * @param nprocs      Number of processes.
 * @param nclusters   Number of clusters.
 * @param use_auction Use auction balancer?
 *
 * @returns A process map.
 */
static int *map_kmeans
(const vector_t *procs, unsigned nprocs, unsigned nclusters, unsigned use_auction)
{
	int *map;            /* Process map.                  */
	int *balanced_map;   /* Balanced process map.         */
	double *avg;         /* Average distance in clusters. */
	vector_t *centroids; /* Centroids.                    */

	map = kmeans(procs, nprocs, nclusters, 0.0);

	centroids = compute_centroids(procs, nprocs, map, nclusters);
	avg = compute_average_distance(procs, nprocs, centroids, nclusters, map);
	
	/* Print average distance. */
	for (unsigned i = 0; i < nclusters; i++)
		fprintf(stderr, "cluster %d: %lf\n", i, avg[i]);
	fprintf(stderr, "\n");
	
	/* Balance. */
	if (use_auction)
	{
		free(avg);
		balanced_map = auction_balance(procs, nprocs, centroids, nclusters);
		destroy_centroids(centroids, nclusters);
		centroids = compute_centroids(procs, nprocs, balanced_map, nclusters);
		avg = compute_average_distance(procs, nprocs, centroids, nclusters, map);
		map = balanced_map;
	}
	
	/* Print average distance. */
	for (unsigned i = 0; i < nclusters; i++)
		fprintf(stderr, "cluster %d: %lf\n", i, avg[i]);

	/* House keeping. */
	destroy_centroids(centroids, nclusters);
	free(centroids);
	free(avg);

	return (map);
}

/*===========================================================================*
 *                               Public Interface                            *
 *===========================================================================*/

/**
 * @brief Maps process.
 */
int *process_map(unsigned nprocs, matrix_t communication, unsigned nclusters, unsigned use_auction)
{
	int *map;        /* Map.       */
	vector_t *procs; /* Processes. */
	
	/* Create processes. */
	procs = smalloc(nprocs*sizeof(vector_t));
	for (unsigned i = 0; i < nprocs; i++)
	{
		procs[i] = vector_create(nprocs);
		
		for (unsigned j = 0; j < nprocs; j++)
		{
			double a;
			
			vector_set(procs[i], j, 
					((a = matrix_get(communication, i, j)) > 0) ? 1.0/a : a);
		}
	}

	map = map_kmeans(procs, nprocs, nclusters, use_auction);

	/* House keeping. */
	for (unsigned i = 0; i < nprocs; i++)
		vector_destroy(procs[i]);
	free(procs);
	
	return (map);
}
