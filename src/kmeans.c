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
#include <string.h>
#include <stdlib.h>

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

/**
 * @brief Balances processes evenly among clusters using auction's algorithm.
 * 
 * @param procs  Processes.
 * @param nprocs Number of processes.
 * @param map    Unbalanced process map.
 * 
 * @returns A balanced process map.
 */
static int *balance_auction(const vector_t *procs, int nprocs, int *map)
{
	matrix_t m;            /* Auction's matrix.   */
	int *balanced_map;     /* Process map.        */
	int nclusters;         /* Number of clusters. */
	vector_t *centroids;   /* Centroids.          */
	int procs_per_cluster; /* Gotcha?             */
	
	centroids = kmeans_centroids(procs, nprocs, map);
	nclusters = kmeans_count_centroids(map, nprocs);
	
	if (nprocs%nclusters)
		error("invalid number of clusters");
	
	procs_per_cluster = nprocs/nclusters;
	
	m = matrix_create(nprocs, nprocs);
	
	/* Build auction's matrix. */
	for (int i = 0; i < nprocs; i++)
	{
		for (int j = 0; j < nclusters; j++)
		{
			double distance;
			
			distance = vector_distance(procs[i], centroids[j]);
			for (int k = 0; k < procs_per_cluster; k++)
				matrix_set(m, i, procs_per_cluster*j + k, distance);
		}
	}
	
	balanced_map = auction(m, 0.0001);
	
	/* Fix map. */
	for (int i = 0; i < nprocs; i++)
		balanced_map[i] /= procs_per_cluster;
	
	/* House keeping. */
	matrix_destroy(m);
	destroy_centroids(centroids, nclusters);
	
	return (balanced_map);
}

/**
 * @brief Balances processes evenly among clusters using a greedy strategy.
 * 
 * @param procs  Processes.
 * @param nprocs Number of processes.
 * @param map    Unbalanced process map.
 * 
 * @returns A balanced process map.
 */
static int *balance_greedy(const vector_t *procs, int nprocs, int *map)
{
	int *balanced_map;     /* Process map.        */
	int nclusters;         /* Number of clusters. */
	vector_t *centroids;   /* Centroids.          */
	int procs_per_cluster; /* Gotcha?             */
	
	centroids = kmeans_centroids(procs, nprocs, map);
	nclusters = kmeans_count_centroids(map, nprocs);
	
	if (nprocs%nclusters)
		error("invalid number of clusters");
	
	procs_per_cluster = nprocs/nclusters;
	
	balanced_map = smalloc(nprocs*sizeof(int));
	memcpy(balanced_map, map, nprocs*sizeof(int));
	
	/* Balance. */
	for (int i = 0; i < nclusters; i++)
	{
		int n1 = kmeans_count_points(balanced_map, nprocs, i);
		
		/* Take a processes out from this cluster. */
		while (n1-- > procs_per_cluster)
		{
			int farthest;
			double d1, d2;
			
			/* Get farthest process. */
			farthest = -1;
			for (int j = 0; j < nprocs; j++)
			{
				if (balanced_map[j] != i)
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
				if ((kmeans_count_points(balanced_map, nprocs, j)) >= procs_per_cluster)
					continue;
				
				balanced_map[farthest] = j;
			}
		}
	}
	
	/* House keeping. */
	destroy_centroids(centroids, nclusters);
	
	return (balanced_map);
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
int *map_kmeans
(const vector_t *procs, unsigned nprocs, void *args)
{
	int *map;             /* Process map.                  */
	int *balanced_map;     /* Balanced process map.        */
	unsigned use_auction; /* Use auction balancing?        */
	unsigned nclusters;   /* Number of clusters.           */
	double *avg;          /* Average distance in clusters. */
	
	/* Extract arguments. */
	nclusters = ((struct kmeans_args *)args)->nclusters;
	use_auction = ((struct kmeans_args *)args)->use_auction;
	
	/* Sanity check. */
	assert(nclusters > 0);
	
	map = kmeans(procs, nprocs, nclusters, 0.0);
	
	/* Balance. */
	balanced_map = (use_auction) ?
		balance_auction(procs, nprocs, map) :
		balance_greedy(procs, nprocs, map);
	
	/* Print average distance. */
	if (verbose)
	{
		vector_t *centroids;
		centroids = kmeans_centroids(procs, nprocs, map);
		avg = kmeans_average_distance(procs, nprocs, centroids, nclusters, map);
		for (unsigned i = 0; i < nclusters; i++)
			fprintf(stderr, "cluster %d: %.10lf\n", i, avg[i]);
		fprintf(stderr, "\n");
		free(avg);
		destroy_centroids(centroids, nclusters);
	}

	/* House keeping. */
	free(map);
	
	return (balanced_map);
}
