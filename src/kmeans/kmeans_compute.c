/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans/kmeans_compute.c> - kmeans_compute() implementation.
 */

#include <assert.h>
#include <list.h>
#include <stdlib.h>
#include <vector.h>
#include "kmeans.h"

/*
 * Compute the means of the clusters.
 */
int kmeans_compute(void)
{
	unsigned i, j;
	int has_changed;
	struct vector *mean;
	struct list_node *n;
	
	has_changed = 0;
	
	/* Compute means. */
	for (i = 0; i < nclusters; i++)
	{
		mean = vector_create(nprocs*nprocs);
		assert(mean != NULL);
			
		/* Compute cluster's mean. */
		for (n = list_head(clusters[i].procs); n != NULL; n = list_next(n))
		{
			for (j = 0; j < nprocs; j++)
				VECTOR(mean, j) += VECTOR(PROCESS(n)->traffic, j);
		}
		for (j = 0; j < nprocs; j++)
			VECTOR(mean, j) /= list_length(clusters[i].procs);
			
		/* Cluster mean has changed. */
		if (!vector_equal(clusters[i].mean, mean))
			has_changed = 1;
		
		vector_destroy(clusters[i].mean);
		clusters[i].mean = mean;	
	}
	
	return (has_changed);
}
