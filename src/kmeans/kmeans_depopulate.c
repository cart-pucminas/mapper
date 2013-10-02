/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans/kmeans_depopulate.c> - kmeans_depopulate() implementation.
 */

#include <list.h>
#include "kmeans.h"

/*
 * Depopulate clusters.
 */
void kmeans_depopulate(void)
{
	int i;
	struct list_node *n;
	
	/* Depupulate clusters. */
	for (i = 0; i < nclusters; i++)
	{
		while (list_length(clusters[i].procs) > 0)
		{
			n = list_remove_first(clusters[i].procs);
			list_insert(procs, n);
		}
	}
}
