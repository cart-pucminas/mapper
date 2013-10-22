/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans/kmeans_write.c> - kmeans_write() implementation.
 */

#include <list.h>
#include <stdio.h>
#include "kmeans.h"

/*
 * Writes kmeans() result in a file.
 */
void kmeans_write(FILE *output)
{
	int i;
	int processor;
	struct list_node *n;
	
	processor = 0;
	
	/* Map processes. */
	for (i = 0; i < nclusters; i++)
	{
		for (n = list_head(clusters[i].procs); n != NULL; n = list_next(n))
		{
			fprintf(output, "%d %d\n", PROCESS(n)->id, processor);			

			/* FIXME: Only works for 4x8 NoC. */
			processor += 8;
			if (processor >= 32)
				processor = processor%32 + 1;
		}
	}
}
