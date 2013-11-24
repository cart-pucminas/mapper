/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans/kmeans_init.c> - kmeans_init() implementation.
 */

#include <assert.h>
#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector.h>
#include "kmeans.h"

/*
 * Initializes internal data structures.
 */
void kmeans_init(FILE *input, int _nclusters)
{
	int i, j;
	struct list_node *n;
	
	nclusters = _nclusters;
	
	kmeans_read(input);

	assert((nprocs%nclusters) == 0);
	
	/* Create clusters. */
	clusters = malloc(nclusters*sizeof(struct cluster));
	assert(clusters != NULL);
	
	/* Initialize clusters. */
	for (i = 0; i < nclusters; i++)
	{
		clusters[i].size = 0;
		clusters[i].procs = list_create();
		assert(clusters[i].procs != NULL);
		clusters[i].mean = vector_create(nprocs*nprocs);
		assert(clusters[i].mean != NULL);
	}

	/* Distribute processes among clusters. */
	for (i = 0, j = 0; i < nprocs; i++, j = (j + 1)%nclusters)
	{
		n = list_remove_first(procs);
		PROCESS(n)->old = &clusters[j];
		list_insert(clusters[j].procs, n);
		clusters[j].size++;
	}
	
	/* Compute initial means. */
	kmeans_compute();
}

