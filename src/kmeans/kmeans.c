/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans.c> - kmeans() implementation.
 */

#include <stdio.h>
#include <list.h>
#include "kmeans.h"

/* Number of processes. */
int nprocs = 0;

/* Processes. */
struct list *procs = NULL;

/* Number of clusters. */
int nclusters = 0;

/* Clusters. */
struct cluster *clusters = NULL; 

/*
 *  k-means algorithm clusterization.
 */
void kmeans(int _nclusters, int mindistance, FILE *input, FILE *output)
{
	int i;
	int it;
	int too_far;
	int has_changed;
	struct list_node *walker;
	
	it = 0;
	
	kmeans_init(input, _nclusters);
	
	printf("NoC topology: %dx%d\n", noc.height, noc.width);
	printf("Number of clusters: %d\n", nclusters);
	printf("Minimum distance: %d\n", mindistance);	
	
	printf("Initial clusters:\n");
	for (i = 0; i < nclusters; i++)
	{
		printf("  Cluster %d: ", i);
		for (walker = list_head(clusters[i].procs); walker != NULL; walker = list_next(walker))
			printf("%d ", PROCESS(walker)->id);
		printf("\n");
	}
	
	/* Apply custerization algorithm. */
	do
	{
		kmeans_depopulate();
		
		too_far = kmeans_populate(mindistance);
		
		has_changed = kmeans_compute();
		
		it++;
		
	} while (too_far && has_changed);
	
	printf("Clustering done in %d iterations %s %s\n", it,
		(too_far) ? "" : "(minimum distance achieved)", 
		(has_changed) ? "" : "(centroids did not changed)");
	
	printf("After clustering:\n");
	for (i = 0; i < nclusters; i++)
	{
		printf("  Cluster %d: ", i);
		for (walker = list_head(clusters[i].procs); walker != NULL; walker = list_next(walker))
			printf("%d ", PROCESS(walker)->id);
		printf("\n");
	}
	
	kmeans_balance();

	printf("After balancing clusters:\n");
	for (i = 0; i < nclusters; i++)
	{
		printf("  Cluster %d: ", i);
		for (walker = list_head(clusters[i].procs); walker != NULL; walker = list_next(walker))
			printf("%d ", PROCESS(walker)->id);
		printf("\n");
	}
	
	kmeans_write(output);
	
	kmeans_destroy();
}
