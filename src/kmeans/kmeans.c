/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans.c> - kmeans() implementation.
 */

#include <stdio.h>
#include "kmeans.h"

/* Number of processes. */
int nprocs = 0;

/* Processes. */
struct list *procs = NULL;

/* Number of clusters. */
int nclusters = 0;

/* Clusters. */
struct cluster *clusters = 0; 

/*
 *  k-means algorithm clusterization.
 */
void kmeans(int _nclusters, int mindistance, FILE *input, FILE *output)
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
	
	kmeans_write(output);
	
	kmeans_destroy();
}
