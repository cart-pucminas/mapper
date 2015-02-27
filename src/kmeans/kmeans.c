/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans.c> - kmeans() implementation.
 */

#include <stdio.h>
#include <list.h>
#include "kmeans.h"

/* Number of processes. */
unsigned nprocs = 0;

/* Processes. */
struct list *procs = NULL;

/* Number of clusters. */
unsigned nclusters = 0;

/* Clusters. */
struct cluster *clusters = NULL; 

/*
 *  k-means algorithm clusterization.
 */
void kmeans(unsigned _nclusters, unsigned mindistance, FILE *input)
{
	int too_far;
	int has_changed;
	
	kmeans_init(input, _nclusters);
	
	/* Apply custerization algorithm. */
	do
	{
		kmeans_depopulate();
		
		too_far = kmeans_populate(mindistance);
		
		has_changed = kmeans_compute();
	} while (too_far && has_changed);
	
	kmeans_balance();

	kmeans_write(input);
	
	kmeans_destroy();
}
