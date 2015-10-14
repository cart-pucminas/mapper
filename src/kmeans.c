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

#include <limits.h>

#include <mylib/algorithms.h>
#include <mylib/matrix.h>
#include <mylib/vector.h>
#include <mylib/ai.h>
#include <mylib/util.h>
 
#include "mapper.h"

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
	free(centroids);
}

#ifdef _KMEANS_

/**
 * @brief Balances processes evenly among clusters using auction's algorithm.
 * 
 * @param procs     Processes.
 * @param nprocs    Number of processes.
 * @param centroids Centroids of clusters.
 * @param nclusters Number of clusters.
 */
static int *balance
(const vector_t *procs, unsigned nprocs)
{
	int *map;                                       /* Process map.        */
	matrix_t m;                                     /* Auction's matrix.   */
	unsigned nclusters;                             /* Number of clusters. */
	const vector_t *centroids;                      /* Centroids.          */
	const int procs_per_cluster = nprocs/nclusters; /* Gotcha?             */
	
	if (nprocs%nclusters)
		error("invalid number of clusters");
	
	m = matrix_create(nprocs, nprocs);
	
	/* Build auction's matrix. */
	for (unsigned i = 0; i < nprocs; i++)
	{
		for (unsigned j = 0; j < nclusters; j++)
		{
			double distance;
			
			distance = vector_distance(procs[i], centroids[j]);
			for (unsigned k = 0; k < procs_per_cluster; k++)
				matrix_set(m, i, procs_per_cluster*j + k, distance);
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

#else

/**
 * @brief Balances processes among clusters using a greedy strategy
 * 
 * @param procs     Processes.
 * @param nprocs    Number of processes.
 * @param centroids Centroids of clusters.
 * @param nclusters Number of clusters.
 * @param map       Process map.
 */
static void greedy_balance
(const vector_t *procs, int nprocs, const vector_t *centroids, int nclusters, int *map)
{
	const int procs_per_cluster = nprocs/nclusters;
	
	if (nprocs%nclusters)
		error("invalid number of clusters");
	
	/* Balance. */
	for (int i = 0; i < nclusters; i++)
	{
		int n1 = kmeans_count(map, nprocs, i);
		
		/* Take a processes out from this cluster. */
		while (n1-- > procs_per_cluster)
		{
			int farthest;
			double d1, d2;
			
			/* Get farthest process. */
			farthest = UINT_MAX;
			for (int j = 0; j < nprocs; j++)
			{
				if (map[j] != i)
					continue;
				
				if (farthest < 0)
				{
					d1 = vector_distance(procs[farthest = j], centroids[i]);
					continue;
				}
				
				if ((d2 = vector_distance(procs[j], centroids[i])) > d1)
					farthest = j, d2 = d1;
			}
			
			/* Get not crowded cluster. */
			for (int j = 0; j < nclusters; j++)
			{
				if ((kmeans_count(map, nprocs, j)) >= procs_per_cluster)
					continue;
				
				map[farthest] = j;
			}
		}
	}
}

#endif

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
int *map_kmeans
(const vector_t *procs, unsigned nprocs, void *args)
{
	int *map;             /* Process map.                  */
	unsigned use_auction; /* Use auction balancing?        */
	unsigned nclusters;   /* Number of clusters.           */
	double *avg;          /* Average distance in clusters. */
	vector_t *centroids;  /* Centroids.                    */
	
	UNUSED(use_auction);
	
	/* Extract arguments. */
	nclusters = ((struct kmeans_args *)args)->nclusters;
	use_auction = ((struct kmeans_args *)args)->use_auction;
	
	/* Sanity check. */
	assert(nclusters > 0);
	
	map = kmeans(procs, nprocs, nclusters, 0.0);
	
	/* Balance. */
	centroids = kmeans_centroids(procs, nprocs, map);
	greedy_balance(procs, nprocs, centroids, nclusters, map);
	
	/* Print average distance. */
	if (verbose)
	{
		destroy_centroids(centroids, nclusters);
		centroids = kmeans_centroids(procs, nprocs, map);
		avg = kmeans_average_distance(procs, nprocs, centroids, nclusters, map);
		for (unsigned i = 0; i < nclusters; i++)
			fprintf(stderr, "cluster %d: %.10lf\n", i, avg[i]);
		fprintf(stderr, "\n");
		free(avg);
	}

	/* House keeping. */
	destroy_centroids(centroids, nclusters);
	
	return (map);
}

