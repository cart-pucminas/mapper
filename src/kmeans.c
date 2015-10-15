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

#include <string.h>
#include <stdlib.h>

#include <mylib/algorithms.h>
#include <mylib/matrix.h>
#include <mylib/vector.h>
#include <mylib/ai.h>
#include <mylib/util.h>
#include <mylib/table.h>
 
#include "mapper.h"

/**
 * @brief Destroys centroids.
 * 
 * @param centroids Centroids.
 * @param ncentroids Number of centroids.
 */
static void destroy_centroids(vector_t *centroids, int ncentroids)
{
	for (int i = 0; i < ncentroids; i++)
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
 * @returns A balanced cluster map.
 */
static int *balance_auction
(const vector_t *procs, int nprocs, int *map, int nclusters)
{
	matrix_t m;            /* Auction's matrix. */
	int *balanced_map;     /* Process map.      */
	vector_t *centroids;   /* Centroids.        */
	int procs_per_cluster; /* Gotcha?           */
	
	centroids = kmeans_centroids(procs, nprocs, map);
	
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
 * @returns A balanced cluster map.
 */
static int *balance_greedy
(const vector_t *procs, int nprocs, int *map, int nclusters)
{
	int *balanced_map;     /* Process map. */
	vector_t *centroids;   /* Centroids.   */
	int procs_per_cluster; /* Gotcha?      */
	
	centroids = kmeans_centroids(procs, nprocs, map);
	
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
 * @brief Internal implementation of table_split().
 */
static void _table_split
(struct table *t, int i0, int j0, int height, int width, int size)
{
	static int count = 0;
	
	/* Stop condition reached. */
	if (width*height <= size)
	{
		/* Enumerate region. */
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				table_set(t, i0 + i, j0 + j, smalloc(sizeof(int)));
				*INTP(table_get(t, i0 + i, j0 + j)) = count;
			}
		}
		
		count++;
		
		return;
	}
	
	/* Split vertically. */
	if (width > height)
	{
		_table_split(t, i0, j0, height, width/2, size);
		_table_split(t, i0, j0 + width/2, height, width/2, size);
	}
	
	/* Split horizontally. */
	else
	{
		_table_split(t, i0, j0, height/2, width, size);
		_table_split(t, i0 + height/2, j0, height/2, width, size);
	}
}

/**
 * @brief Splits a table recursively.
 * 
 * @param t Target table.
 */
static void table_split(struct table *t, int size)
{
	_table_split(t, 0, 0, table_height(t), table_width(t), size);
}

/**
 * @brief Places processes in the processor.
 * 
 * @param mesh       Processor's topology.
 * @param clustermap Process map.
 * @param nprocs     Number of processes.
 * 
 * @returns Process map.
 */
static int *place
(struct topology *mesh, int *clustermap, int nprocs, int nclusters)
{
	int *map;               /* Process map.        */
	struct table *clusters;	/* Clustered topology. */
	
	map = smalloc(nprocs*sizeof(int));
	clusters = table_create(&integer, mesh->height, mesh->width);
	
	table_split(clusters, nprocs/nclusters);
	
	/* Place processes in the processor. */
	for (int i = 0; i < mesh->height; i++)
	{
		for (int j = 0; j < mesh->width; j++)
		{
			int c;
			
			c = *INTP(table_get(clusters, i, j));
			for (int k = 0; k < nprocs; k++)
			{
				if (clustermap[k] == c)
				{
					clustermap[k] = -1;
					map[k] = i*mesh->width + j;
					break;
				}
			}
		}
	}
	
	/* House keeping. */
	table_destroy(clusters);
	
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
int *map_kmeans
(const vector_t *procs, int nprocs, void *args)
{
	int *map;              /* Process map.           */
	int *clustermap;       /* Cluster map.           */
	int *balanced_map;     /* Balanced cluster map.  */
	int use_auction;       /* Use auction balancing? */
	int nclusters;         /* Number of clusters.    */
	struct topology *mesh; /* Processor's topology.  */
	
	/* Extract arguments. */
	nclusters = ((struct kmeans_args *)args)->nclusters;
	use_auction = ((struct kmeans_args *)args)->use_auction;
	mesh = ((struct kmeans_args *)args)->mesh;
	
	/* Sanity check. */
	assert(nclusters > 0);
	assert(nprocs == (mesh->height*mesh->width));
	
	clustermap = kmeans(procs, nprocs, nclusters, 0.0);
	
	/* Balance. */
	balanced_map = (use_auction) ?
		balance_auction(procs, nprocs, clustermap, nclusters) :
		balance_greedy(procs, nprocs, clustermap, nclusters);
	
	map = place(mesh, balanced_map, nprocs, nclusters);

	/* House keeping. */
	free(clustermap);
	free(balanced_map);
	
	return (map);
}
