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
 * @brief Balances processes among clusters using a greedy strategy
 * 
 * @param procs     Processes.
 * @param nprocs    Number of processes.
 * @param centroids Centroids of clusters.
 * @param nclusters Number of clusters.
 * @param map       Process map.
 */
static int *greedy_balance
(const vector_t *procs, int nprocs, const vector_t *centroids, int nclusters, int *map)
{
	if (nprocs%nclusters)
		error("invalid number of clusters");
	
	/* Balance. */
	for (int i = 0; i < nclusters; i++)
	{
		int n1;       /* nprocs/nclusters  */
		int farthest; /* Farthest process. */
		
		n1 = kmeans_count(map, nprocs, i);
		
		/* Take a processes out from this cluster. */
		while (n1-- > (nprocs/nclusters))
		{
			int n2;
			double d1, d2;
			
			/* Get farthest process. */
			farthest = UINT_MAX;
			for (int j = 0; j < nprocs; j++)
			{
				if (map[j] != i)
					continue;
				
				if (farthest < 0)
				{
					farthest = j;
					d1 = vector_distance(procs[farthest], centroids[i]);
					continue;
				}
				
				d2 = vector_distance(procs[j], centroids[i]);
				
				if (d2 > d1)
				{
					farthest = j;
					d2 = d1;
				}
			}
			
			/* Get empty cluster. */
			for (int j = 0; j < nclusters; j++)
			{
				n2 = kmeans_count(map, nprocs, j);
				if (n2 < (nprocs/nclusters))
				{
					map[farthest] = j;
					break;
				}
			}
		}
	}
	
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
(const vector_t *procs, unsigned nprocs, void *args)
{
	int *map;             /* Process map.                  */
	unsigned use_auction; /* Use auction balancing?        */
	unsigned nclusters;   /* Number of clusters.           */
	int *balanced_map;    /* Balanced process map.         */
	double *avg;          /* Average distance in clusters. */
	vector_t *centroids;  /* Centroids.                    */
	
	/* Extract arguments. */
	nclusters = ((struct kmeans_args *)args)->nclusters;
	use_auction = ((struct kmeans_args *)args)->use_auction;
	
	/* Sanity check. */
	assert(nclusters > 0);
	
	map = kmeans(procs, nprocs, nclusters, 0.0);

	centroids = kmeans_centroids(procs, nprocs, map);
	
	/* Balance. */
	balanced_map = (use_auction) ?
		auction_balance(procs, nprocs, centroids, nclusters) :
		greedy_balance(procs, nprocs, centroids, nclusters, map);			
	destroy_centroids(centroids, nclusters);
	centroids = kmeans_centroids(procs, nprocs, balanced_map);
	avg = kmeans_average_distance(procs, nprocs, centroids, nclusters, balanced_map);
	if (use_auction)
		free(map);
	map = balanced_map;
	
	/* Print average distance. */
	if (verbose)
	{
		for (unsigned i = 0; i < nclusters; i++)
			printf("cluster %d: %.10lf\n", i, avg[i]);
		printf("\n");
	}

	/* House keeping. */
	destroy_centroids(centroids, nclusters);
	free(avg);
	
	return (balanced_map);
}

/*===========================================================================*
 *                               Public Interface                            *
 *===========================================================================*/

/**
 * @brief Number of mapping strategies.
 */
#define NR_STRATEGIES 1

/**
 * @brief Mapping strategy.
 */
typedef int *(*strategy)(const vector_t *, unsigned, void *);

/**
 * @brief Mapping strategies.
 */
static strategy strategies[NR_STRATEGIES] =  {
	map_kmeans
};

/**
 * @brief Maps process.
 */
int *process_map(matrix_t communication, unsigned strategy, void *args)
{
	int *map;        /* Map.                 */
	unsigned nprocs; /* Number of processes. */
	vector_t *procs; /* Processes.           */
	
	/* Sanity check. */
	assert(communication != NULL);
	assert(matrix_height(communication) == matrix_width(communication));
	assert(strategy < NR_STRATEGIES);
	assert(args != NULL);
	
	nprocs = matrix_height(communication);
	
	/* Create processes. */
	procs = smalloc(nprocs*sizeof(vector_t));
	for (unsigned i = 0; i < nprocs; i++)
	{
		procs[i] = vector_create(nprocs);
		for (unsigned j = 0; j < nprocs; j++)
		{
			double a;
			a = matrix_get(communication, i, j);
			vector_set(procs[i], j, (a > 0) ? 1.0/a : a);
		}
	}

	map = strategies[strategy](procs, nprocs, args);

	/* House keeping. */
	for (unsigned i = 0; i < nprocs; i++)
		vector_destroy(procs[i]);
	free(procs);
	
	return (map);
}
