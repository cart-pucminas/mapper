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
#include <mylib/queue.h>
 
#include "mapper.h"

#ifdef USE_AUCTION

#define AUCTION_SLACK 0.001

/**
 * @brief Balances processes evenly among clusters using auction's algorithm.
 * 
 * @param kdata Kmeans data.
 * 
 * @returns A balanced cluster map.
 */
static int *balance(struct kmeans_data *kdata)
{
	matrix_t m;            /* Auction's matrix.    */
	int *balanced_map;     /* Process map.         */
	int procs_per_cluster; /* Gotcha?              */
	
	if (kdata->npoints%kdata->ncentroids)
		error("invalid number of clusters");
	
	procs_per_cluster = kdata->npoints/kdata->ncentroids;
	
	m = matrix_create(kdata->npoints, kdata->npoints);
	
	/* Build auction's matrix. */
	for (int i = 0; i < kdata->npoints; i++)
	{
		for (int j = 0; j < kdata->ncentroids; j++)
		{
			double distance;
			
			distance = vector_distance(kdata->data[i], kdata->centroids[j]);
			for (int k = 0; k < procs_per_cluster; k++)
				matrix_set(m, i, procs_per_cluster*j + k, distance);
		}
	}
	
	balanced_map = auction(m, AUCTION_SLACK);
	
	/* Fix map. */
	for (int i = 0; i < kdata->npoints; i++)
		balanced_map[i] /= procs_per_cluster;
	
	/* House keeping. */
	matrix_destroy(m);
	
	return (balanced_map);
}

#else

/**
 * @brief Balances processes evenly among clusters using a greedy strategy.
 * 
 * @param kdata Kmeans data.
 * 
 * @returns A balanced cluster map.
 */
static int *balance(struct kmeans_data *kdata)
{
	int *balanced_map;     /* Process map. */
	int procs_per_cluster; /* Gotcha?      */
	
	if (kdata->npoints%kdata->ncentroids)
		error("invalid number of clusters");
	
	procs_per_cluster = kdata->npoints/kdata->ncentroids;
	
	balanced_map = smalloc(kdata->npoints*sizeof(int));
	memcpy(balanced_map, kdata->map, kdata->npoints*sizeof(int));
	
	/* Balance. */
	for (int i = 0; i < kdata->ncentroids; i++)
	{
		int n1 = kmeans_count_points(balanced_map, kdata->npoints, i);
		
		/* Take a processes out from this cluster. */
		while (n1-- > procs_per_cluster)
		{
			int farthest;
			double d1, d2;
			
			/* Get farthest process. */
			farthest = -1;
			for (int j = 0; j < kdata->npoints; j++)
			{
				if (balanced_map[j] != i)
					continue;
				
				if (farthest < 0)
				{
					farthest = j;
					d1 = vector_distance(kdata->data[j], kdata->centroids[i]);
					continue;
				}
				
				if ((d2 = vector_distance(kdata->data[j], kdata->centroids[i])) > d1)
					farthest = j, d2 = d1;
			}
			
			/* Get not crowded cluster. */
			for (int j = 0; j < kdata->ncentroids; j++)
			{
				if ((kmeans_count_points(balanced_map, kdata->npoints, j)) >= procs_per_cluster)
					continue;
				
				balanced_map[farthest] = j;
			}
		}
	}
	
	return (balanced_map);
}

#endif

/**
 * @brief Internal implementation of table_split().
 */
static void _table_split
(struct table *t, int i0, int j0, int height, int width, int size, int depth)
{
	/* Stop condition reached. */
	if (width*height <= size)
		return;
	
	/* Split vertically. */
	if (width > height)
	{
		/* Enumerate region. */
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width/2; j++)
				*INTP(table_get(t, i0 + i, j0 + j)) |= 0 << depth;
			for (int j = width/2; j < width; j++)
				*INTP(table_get(t, i0 + i, j0 + j)) |= 1 << depth;
		}
		
		_table_split(t, i0, j0, height, width/2, size, depth + 1);
		_table_split(t, i0, j0 + width/2, height, width/2, size, depth + 1);
	}
	
	/* Split horizontally. */
	else
	{
		/* Enumerate region. */
		for (int j = 0; j < width; j++)
		{
			for (int i = 0; i < height/2; i++)
				*INTP(table_get(t, i0 + i, j0 + j)) |= 0 << depth;
			for (int i = height/2; i < width; i++)
				*INTP(table_get(t, i0 + i, j0 + j)) |= 1 << depth;
		}
		
		_table_split(t, i0, j0, height/2, width, size, depth + 1);
		_table_split(t, i0 + height/2, j0, height/2, width, size, depth + 1);
	}
}

/**
 * @brief Splits a table recursively.
 * 
 * @param t Target table.
 */
static void table_split(struct table *t, int size)
{
	/* Create table elements. */
	for (unsigned i = 0; i < table_height(t); i++)
	{
		for (unsigned j = 0; j < table_width(t); j++)
			table_set(t, i, j, scalloc(1, sizeof(int)));
	}
		
	_table_split(t, 0, 0, table_height(t), table_width(t), size, 0);
}

/**
 * @brief Places processes in the processor.
 * 
 * @param proc       Processor's information.
 * @param clustermap Process map.
 * @param nprocs     Number of processes.
 * 
 * @returns Process map.
 */
static int *place
(struct processor *proc, int *clustermap, int nprocs, int nclusters)
{
	int *map;               /* Process map.        */
	struct table *clusters;	/* Clustered topology. */
	
	map = smalloc(nprocs*sizeof(int));
	clusters = table_create(&integer, proc->height, proc->width);
	
	table_split(clusters, nprocs/nclusters);
	
	/* Place processes in the processor. */
	for (int i = 0; i < proc->height; i++)
	{
		for (int j = 0; j < proc->width; j++)
		{
			int c;
			
			c = *INTP(table_get(clusters, i, j));
			for (int k = 0; k < nprocs; k++)
			{
				if (clustermap[k] == c)
				{
					clustermap[k] = -1;
					map[k] = i*proc->width + j;
					break;
				}
			}
		}
	}
	
	/* House keeping. */
	for (int i = 0; i < proc->height; i++)
	{
		for (int j = 0; j < proc->width; j++)
			free(table_get(clusters, i, j));
	}
	table_destroy(clusters);
	
	return (map);
}

/**
 * @brief (Balanced) Kmeans clustering.
 * 
 * @param data Data that shall be clustered.
 * @param npoints Number of points that shall be clustered.
 * @param ncentroids Number of centroids
 * 
 * @returns A map that indicates in which cluster each data point is located.
 */
static int *kmeans_balanced(const vector_t *data, int npoints, int ncentroids)
{
	int *balanced_map;         /* Balanced cluster map. */
	struct kmeans_data *kdata; /* Kmeans data.          */
	
	kdata = kmeans(data, npoints, ncentroids, 0.0);
	balanced_map = balance(kdata);
		
	/* House keeping. */
	kmeans_data_destroy(kdata);
	
	return (balanced_map);
}

/**
 * @brief Task.
 */
struct task
{
	int mask;
	int depth;
	int npoints;
	int *ids; 
	vector_t *data;
};

/**
 * @brief Creates task.
 */
static struct task *task_create(int depth, unsigned mask, int npoints)
{
	struct task *t;
	
	t = smalloc(sizeof(struct task));
	t->data = smalloc(npoints*sizeof(vector_t));
	t->depth = depth;
	t->mask = mask;
	t->ids = smalloc(npoints*sizeof(int));
	t->npoints = npoints;
	
	return (t);
}

/**
 * @brief Destroys a task.
 */
static void task_destroy(struct task *t)
{
	free(t->data);
	free(t->ids);
	free(t);
}

/**
 * @brief (Hierarchical) Kmeans clustering.
 * 
 * @param data Data that shall be clustered.
 * @param npoints Number of points that shall be clustered.
 * @param ncentroids Number of centroids
 * 
 * @returns A map that indicates in which cluster each data point is located.
 */
static int *kmeans_hierarchical(const vector_t *data, int npoints)
{
	queue_t tasks;   /* Tasks.              */
	struct task *t;  /* Working task.       */
	int *clustermap; /* Current clustermap. */
	
	clustermap = smalloc(npoints*sizeof(int));
	tasks = queue_create(NULL);
	
	/* Create task. */
	t = task_create(0, 0, npoints);
	for (int i = 0; i < npoints; i++)
		t->ids[i] = i, t->data[i] = data[i];
	
	queue_enqueue(tasks, t);
	
	/* Walk through the hierarychy. */
	while (!queue_empty(tasks))
	{
		int *partialmap;
		
		t = queue_dequeue(tasks);
		
		partialmap = kmeans_balanced(t->data, t->npoints, 2);
		
		/* Fix cluster map. */
		for (int i = 0; i < t->npoints; i++)
			clustermap[t->ids[i]] = (partialmap[i] << t->depth) | t->mask;
		
		/* Enqueue child tasks. */
		if (t->npoints > 4)
		{
			int mask0, mask1;
			struct task *t0, *t1;
			
			mask0 = (0 << t->depth) | t->mask;
			mask1 = (1 << t->depth) | t->mask;
			
			/* Create tasks. */
			t0 = task_create(t->depth + 1, mask0, t->npoints/2);
			t1 = task_create(t->depth + 1, mask1, t->npoints/2);
			for (int i = 0, i0 = 0, i1 = 0; i < t->npoints; i++)
			{
				if (partialmap[i] == 0)
					t0->ids[i0] = t->ids[i], t0->data[i0++] = data[t->ids[i]];
				else
					t1->ids[i1] = t->ids[i], t1->data[i1++] = data[t->ids[i]];
			}
			
			queue_enqueue(tasks, t0);
			queue_enqueue(tasks, t1);
		}
		
		/* House keeping. */
		free(partialmap);
		task_destroy(t);
	}
	
	/* House keeping. */
	queue_destroy(tasks);
	
	return (clustermap);
}

/**
 * @brief Maps processes using kmeans algorithm.
 *
 * @param communication Communication matrix.
 * @param args          Additional arguments.
 *
 * @returns A process map.
 */
int *map_kmeans(matrix_t communication, void *args)
{
	int *map;               /* Process map.           */
	int *clustermap;        /* Balanced cluster map.  */
	int nclusters;          /* Number of clusters.    */
	int hierarchical;       /* Hierarchical mapping?  */
	struct processor *proc; /* Processor's topology.  */
	int nprocs;             /* Number of processes.   */
	vector_t *procs;        /* Processes.             */
	
	/* Sanity check. */
	assert(communication != NULL);
	assert(args != NULL);
	
	/* Extract arguments. */
	hierarchical = ((struct kmeans_args *)args)->hierarchical;
	nclusters = ((struct kmeans_args *)args)->nclusters;
	proc = ((struct kmeans_args *)args)->proc;
	
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
			if (hierarchical)
				vector_set(procs[i], j, (a > 0) ? 1.0/a : a);
			else
				vector_set(procs[i], j, a);
		}
	}
	
	/* Sanity check. */
	if (!hierarchical)
		assert(nclusters > 0);
	assert(nprocs == (proc->height*proc->width));

	/* Hierarchical kmeans. */
	if (hierarchical)
	{
		clustermap = kmeans_hierarchical(procs, nprocs);
		map = place(proc, clustermap, nprocs, nprocs/2);
	}
	
	/* Standard kmeans. */
	else
	{
		clustermap = kmeans_balanced(procs, nprocs, nclusters);
		map = place(proc, clustermap, nprocs, nclusters);
	}
	
	/* House keeping. */
	free(clustermap);
	for (int i = 0; i < nprocs; i++)
		vector_destroy(procs[i]);
	free(procs);
	
	return (map);
}
