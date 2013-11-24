/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans/kmeans_populate.c> - kmeans_populate() implementation.
 */

#include <list.h>
#include <vector.h>
#include "kmeans.h"

/*
 * Populates clusters.
 */
int kmeans_populate(int mindistance)
{
	int i;
	int too_far;
	struct list_node *n;
	double tmp, distance;
	struct cluster *to_insert;
	
	too_far = 0;
	
	/* Group processes into clusters. */
	while (list_length(procs) > 0)
	{
		n = list_remove_first(procs);
		
		/* Look for closest cluster. */
		to_insert = PROCESS(n)->old;
		distance = vector_distance(PROCESS(n)->traffic, to_insert->mean);
		for (i = 0; i < nclusters; i++)
		{
			if (&clusters[i] == PROCESS(n)->old)
				continue;
			
			tmp = vector_distance(PROCESS(n)->traffic, clusters[i].mean);
			
			/* Found. */
			if (tmp < distance)
			{
				to_insert = &clusters[i];
				distance = tmp;
			}
		}
		
		PROCESS(n)->old = to_insert;
		list_insert(to_insert->procs, n);
		
		/* Cluster is too far away. */
		if (distance > mindistance)
			too_far = 1;
	}
	
	return (too_far);
}
