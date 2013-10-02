/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans/kmeans_balance.c> - kmeans_balance() implementation.
 */

#include <list.h>
#include <stdlib.h>
#include <vector.h>
#include "kmeans.h"

/*
 * Balances number of processes between clusters.
 */
void kmeans_balance(void)
{
	int i, j;
	double tmp;
	double distance;
	struct list_node *n;
	struct list_node *distant;
	struct cluster *to_insert;
	
	/* Check all clusters. */
	for (i = 0; i < nclusters; i++)
	{
		/* Too many processes in this cluster. */
		while (list_length(clusters[i].procs) > clusters[i].size)
		{		
			distant = list_head(clusters[i].procs);
			distance = vector_distance(PROCESS(distant)->traffic, clusters[i].mean);
			
			/* Find the more distant process within the cluster. */
			for (n = list_next(distant); n != NULL; n = list_next(n))
			{
				tmp = vector_distance(PROCESS(n)->traffic, clusters[i].mean);
				if (tmp > distance)
				{
					distance = tmp;
					distant = n;
				}
			}
			
			to_insert = NULL;
			
			/* Find the best cluster for the process. */
			for (j = 0; j < nclusters; j++)
			{
				/* Skip fully populated clusters. */
				if (list_length(clusters[j].procs) >= clusters[j].size)
					continue;
				
				if (to_insert == NULL)
				{
					to_insert = &clusters[j];
					distance = vector_distance(PROCESS(distant)->traffic, to_insert->mean);
					continue;
				}
				
				tmp = vector_distance(PROCESS(distant)->traffic, clusters[j].mean);
				
				/* Found. */
				if (tmp < distance)
				{
					distance = tmp;
					to_insert = &clusters[j];
				}
			}
			
			/* Insert process in the cluster. */
			list_remove(clusters[i].procs, distant);
			list_insert(to_insert->procs, distant);
		}
	}
}
