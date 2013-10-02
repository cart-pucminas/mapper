/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans/kmeans_destroy.c> - kmeans_destroy() implementation.
 */

#include <list.h>
#include <stdlib.h>
#include <vector.h>
#include "kmeans.h"

/*
 * Destroys internal data structures.
 */
void kmeans_destroy(void)
{
	int i;
	struct list_node *n;
	
	/* Deallocate all data from each clusters. */
	for (i = 0; i < nclusters; i++)
	{
		/* Deallocate all processes. */
		while (list_length(clusters[i].procs) > 0)
		{
			n = list_remove_first(clusters[i].procs);
			vector_destroy(PROCESS(n)->traffic);
			free(PROCESS(n));
			list_node_destroy(n);
		}
		
		list_destroy(clusters[i].procs);
		vector_destroy(clusters[i].mean);
	}
	
	list_destroy(procs);
	free(clusters);
}

