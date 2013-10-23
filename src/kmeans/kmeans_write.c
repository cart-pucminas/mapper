/*
 * Copyright (C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * <kmeans/kmeans_write.c> - kmeans_write() implementation.
 */

#include <list.h>
#include <stdio.h>
#include "kmeans.h"

/* FIXME: Only works for 4x8 NoC. */
int map[32] = {
	 0, 1,  8,  9, 16, 17, 24, 25,
	 2, 3, 10, 11, 18, 19, 26, 27,
	 4, 5, 12, 13, 20, 21, 28, 29,
	 6, 7, 14, 15, 22, 23, 30, 31
};

/*
 * Writes kmeans() result in a file.
 */
void kmeans_write(FILE *output)
{
	int i, p;
	struct list_node *n;
	
	p = 0;
	
	/* Map processes. */
	for (i = 0; i < nclusters; i++)
	{
		for (n = list_head(clusters[i].procs); n != NULL; n = list_next(n))
			fprintf(output, "%d %d\n", PROCESS(n)->id, map[p++]);
	}
}
